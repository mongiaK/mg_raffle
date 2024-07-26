local redis = require("resty.redis")

local M = {}

-- Redis 连接配置
local redis_config = {
	host = "127.0.0.1",
	port = 6379,
	password = nil, -- 如果需要密码，请在这里设置
	database = 0, -- 默认数据库
	timeout = 1000, -- 连接超时时间（毫秒）
	pool = {
		max_idle_timeout = 10000, -- 连接的最大空闲时间（毫秒）
		pool_size = 100, -- 连接池大小
	},
}

-- 获取 Redis 连接
local function get_redis()
	local red = redis:new()
	red:set_timeout(redis_config.timeout)

	local ok, err = red:connect(redis_config.host, redis_config.port)
	if not ok then
		return nil, "failed to connect to redis: " .. (err or "unknown error")
	end

	-- 如果设置了密码，进行身份验证
	if redis_config.password then
		local res, err = red:auth(redis_config.password)
		if not res then
			return nil, "failed to authenticate: " .. (err or "unknown error")
		end
	end

	-- 选择数据库
	local res, err = red:select(redis_config.database)
	if not res then
		return nil, "failed to select database: " .. (err or "unknown error")
	end

	return red
end

-- 释放 Redis 连接（放回连接池）
local function release_redis(red)
	if not red then
		return
	end
	local ok, err = red:set_keepalive(redis_config.pool.max_idle_timeout, redis_config.pool.pool_size)
	if not ok then
		ngx.log(ngx.ERR, "failed to set keepalive: ", err)
	end
end

-- GET 操作
function M.get(key)
	local red, err = get_redis()
	if not red then
		return nil, err
	end

	local res, err = red:get(key)
	release_redis(red)

	if err then
		return nil, "redis get error: " .. err
	end

	return res
end

-- SET 操作
function M.set(key, value, expiration)
	local red, err = get_redis()
	if not red then
		return nil, err
	end

	local res, err
	if expiration then
		res, err = red:setex(key, expiration, value)
	else
		res, err = red:set(key, value)
	end

	release_redis(red)

	if err then
		return nil, "redis set error: " .. err
	end

	return res
end

-- DEL 操作
function M.del(key)
	local red, err = get_redis()
	if not red then
		return nil, err
	end

	local res, err = red:del(key)
	release_redis(red)

	if err then
		return nil, "redis del error: " .. err
	end

	return res
end

-- HGET 操作
function M.hget(key, field)
	local red, err = get_redis()
	if not red then
		return nil, err
	end

	local res, err = red:hget(key, field)
	release_redis(red)

	if err then
		return nil, "redis hget error: " .. err
	end

	return res
end

-- HSET 操作
function M.hset(key, field, value)
	local red, err = get_redis()
	if not red then
		return nil, err
	end

	local res, err = red:hset(key, field, value)
	release_redis(red)

	if err then
		return nil, "redis hset error: " .. err
	end

	return res
end

-- LPUSH 操作
function M.lpush(key, value)
	local red, err = get_redis()
	if not red then
		return nil, err
	end

	local res, err = red:lpush(key, value)
	release_redis(red)

	if err then
		return nil, "redis lpush error: " .. err
	end

	return res
end

-- RPOP 操作
function M.rpop(key)
	local red, err = get_redis()
	if not red then
		return nil, err
	end

	local res, err = red:rpop(key)
	release_redis(red)

	if err then
		return nil, "redis rpop error: " .. err
	end

	return res
end

-- 执行自定义命令
function M.command(cmd, ...)
	local red, err = get_redis()
	if not red then
		return nil, err
	end

	local res, err = red[cmd](red, ...)
	release_redis(red)

	if err then
		return nil, "redis command error: " .. err
	end

	return res
end

local function example()
	local redis_ops = M

	-- 使用 SET 和 GET
	local ok, err = redis_ops.set("mykey", "Hello Redis", 3600) -- 设置过期时间为 1 小时
	if not ok then
		ngx.log(ngx.ERR, "Failed to set: ", err)
	end

	local value, err = redis_ops.get("mykey")
	if value then
		ngx.say("Value: ", value)
	else
		ngx.log(ngx.ERR, "Failed to get: ", err)
	end

	-- 使用 HSET 和 HGET
	redis_ops.hset("myhash", "field1", "value1")
	local hvalue, err = redis_ops.hget("myhash", "field1")

	-- 使用列表操作
	redis_ops.lpush("mylist", "item1")
	local item, err = redis_ops.rpop("mylist")

	-- 执行自定义命令
	local result, err = redis_ops.command("INCR", "mycounter")
end

return M
