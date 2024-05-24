package main

import (
	"fmt"
	"github.com/gosnmp/gosnmp"
	"log"
	"net"
	"sync"
)

var packetPool = sync.Pool{
	New: func() interface{} {
		return make([]byte, 1024)
	},
}

type SNMPAgent struct {
	conn           *net.UDPConn
	goSNMP         *gosnmp.GoSNMP
	clientRequests map[string]chan *gosnmp.SnmpPacket // 存储客户端请求
	mutex          sync.Mutex
}

func NewSNMPAgent() (*SNMPAgent, error) {
	// 创建一个 UDP 连接，用于接收 SNMP 请求
	addr := net.UDPAddr{
		Port: 161,
		IP:   net.ParseIP("0.0.0.0"), // 监听所有网络接口
	}

	conn, err := net.ListenUDP("udp", &addr)
	if err != nil {
		return nil, fmt.Errorf("Error listening: %v", err)
	}

	// 创建一个 SNMP agent
	goSNMP := gosnmp.NewGoSNMP(gosnmp.Version2c, nil, nil)

	return &SNMPAgent{
		conn:           conn,
		goSNMP:         goSNMP,
		clientRequests: make(map[string]chan *gosnmp.SnmpPacket),
	}, nil
}

func (agent *SNMPAgent) ListenAndServe() {
	defer agent.conn.Close()

	// 处理 SNMP 请求
	for {
		buf := packetPool.Get().([]byte)
		n, addr, err := agent.conn.ReadFromUDP(buf)
		if err != nil {
			fmt.Println("Error: ", err)
			packetPool.Put(buf)
			continue
		}

		// 将接收到的包放入客户端请求的通道中，以便异步处理
		packet := make([]byte, n)
		copy(packet, buf[:n])

		agent.mutex.Lock()
		clientChan, ok := agent.clientRequests[addr.String()]
		if !ok {
			clientChan = make(chan *gosnmp.SnmpPacket, 10)
			agent.clientRequests[addr.String()] = clientChan
			go agent.handleClient(addr.String(), clientChan)
		}
		agent.mutex.Unlock()

		clientChan <- &gosnmp.SnmpPacket{Data: packet}
		packetPool.Put(buf)
	}
}

func (agent *SNMPAgent) handleClient(clientAddr string, clientChan chan *gosnmp.SnmpPacket) {
	defer func() {
		agent.mutex.Lock()
		delete(agent.clientRequests, clientAddr)
		agent.mutex.Unlock()
	}()

	for packet := range clientChan {
		// 解析 SNMP 请求
		snmpPacket := gosnmp.SnmpPacket{}
		err := snmpPacket.UnmarshalBER(packet.Data)
		if err != nil {
			fmt.Println("Error parsing SNMP packet: ", err)
			continue
		}

		// 处理 GET 请求
		if snmpPacket.PDUType == gosnmp.GetRequest {
			response, err := agent.goSNMP.Get([]string{"1.3.6.1.2.1.1.1.0"})
			if err != nil {
				fmt.Println("Error responding to GET request: ", err)
				continue
			}

			// 构造 SNMP 响应
			responsePacket := gosnmp.SnmpPacket{
				Version:   agent.goSNMP.Version,
				Community: agent.goSNMP.Community,
				PDUType:   gosnmp.GetResponse,
				RequestID: snmpPacket.RequestID,
				Variables: response.Variables,
			}

			// 将 SNMP 响应发送回客户端
			err = responsePacket.MarshalBER(nil)
			if err != nil {
				fmt.Println("Error marshalling SNMP packet: ", err)
				continue
			}
			_, err = agent.conn.WriteToUDP(responsePacket.Bytes(), packet.Addr)
			if err != nil {
				fmt.Println("Error writing SNMP response: ", err)
				continue
			}
		}
	}
}

func main() {
	agent, err := NewSNMPAgent()
	if err != nil {
		log.Fatal(err)
	}
	agent.ListenAndServe()
}
