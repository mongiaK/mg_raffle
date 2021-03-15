/*================================================================
*
*  文件名称：main.go
*  创 建 者: mongia
*  创建日期：2021年03月15日
*
================================================================*/

package main

import (
	"flag"
	"fmt"
	"log"
	"net/http"

	"golang.org/x/net/websocket"
)

var (
	address = flag.String("address", ":12345", "监听的地址")
)

// Echo the data received on the WebSocket.
func EchoServer(ws *websocket.Conn) {
	var msg = make([]byte, 512)
	n, err := ws.Read(msg)
	if err != nil {
		log.Fatal(err)
	}

	fmt.Printf("Received: %s.\n", msg[:n])

	if _, err := ws.Write([]byte("server ok\n")); err != nil {
		log.Fatal(err)
	}
}

// This example demonstrates a trivial echo server.
func main() {
	flag.Parse()

	http.Handle("/mongia/ws", websocket.Handler(EchoServer))
	err := http.ListenAndServe(*address, nil)
	if err != nil {
		panic("ListenAndServe: " + err.Error())
	}
}
