local mysql = require("resty.mysql")

local M = {}

-- 数据库连接配置
local db_config = {
	host = "127.0.0.1",
	port = 3306,
	database = "your_database",
	user = "your_username",
	password = "your_password",
	max_packet_size = 1024 * 1024,
	pool = "my_mysql_pool", -- 连接池名称
	pool_size = 100, -- 连接池大小
	backlog = 10000, -- 等待队列大小
}

-- 获取数据库连接
local function get_db()
	local db, err = mysql:new()
	if not db then
		return nil, "failed to instantiate mysql: " .. (err or "unknown error")
	end

	local ok, err = db:connect(db_config)
	if not ok then
		return nil, "failed to connect to database: " .. (err or "unknown error")
	end

	-- 检查连接是否是从连接池中获取的
	if db:get_reused_times() == 0 then
		-- 新建连接，可能需要进行一些初始化操作
		-- 例如，设置字符集等
		db:query("SET NAMES utf8")
	end

	return db
end

-- 构建 WHERE 子句
local function build_where_clause(conditions)
	if not conditions or next(conditions) == nil then
		return "", {}
	end

	local clauses = {}
	local values = {}

	for k, v in pairs(conditions) do
		if type(v) == "table" then
			local operator = v[1]:upper()
			local value = v[2]

			if operator == "IN" then
				local placeholders = {}
				for i = 1, #value do
					table.insert(placeholders, "?")
					table.insert(values, value[i])
				end
				table.insert(clauses, string.format("%s IN (%s)", k, table.concat(placeholders, ", ")))
			elseif operator == "BETWEEN" then
				table.insert(clauses, string.format("%s BETWEEN ? AND ?", k))
				table.insert(values, value[1])
				table.insert(values, value[2])
			elseif operator == "LIKE" then
				table.insert(clauses, string.format("%s LIKE ?", k))
				table.insert(values, value)
			else -- >, <, >=, <=, <>, etc.
				table.insert(clauses, string.format("%s %s ?", k, operator))
				table.insert(values, value)
			end
		else
			table.insert(clauses, k .. " = ?")
			table.insert(values, v)
		end
	end

	return " WHERE " .. table.concat(clauses, " AND "), values
end

-- SELECT 操作
function M.select(params)
	local db, err = get_db()
	if not db then
		return nil, err
	end

	local fields = params.fields or "*"
	local table_name = params.table
	local where_clause, where_values = build_where_clause(params.conditions)
	local limit_clause = ""
	local order_clause = ""

	if params.limit then
		limit_clause = " LIMIT " .. params.limit
	end

	if params.order then
		order_clause = " ORDER BY " .. params.order
	end

	local sql = string.format("SELECT %s FROM %s%s%s%s", fields, table_name, where_clause, order_clause, limit_clause)

	local res, err, errno, sqlstate = db:query(sql, where_values)
	db:set_keepalive(10000, 100)

	if not res then
		return nil, "bad result: " .. (err or "unknown error")
	end

	return res
end

-- INSERT 操作
function M.insert(params)
	local db, err = get_db()
	if not db then
		return nil, err
	end

	local table_name = params.table
	local fields = {}
	local values = {}
	local placeholders = {}

	for k, v in pairs(params.data) do
		table.insert(fields, k)
		table.insert(values, v)
		table.insert(placeholders, "?")
	end

	local sql = string.format(
		"INSERT INTO %s (%s) VALUES (%s)",
		table_name,
		table.concat(fields, ", "),
		table.concat(placeholders, ", ")
	)

	local res, err, errno, sqlstate = db:query(sql, values)
	db:set_keepalive(10000, 100)

	if not res then
		return nil, "bad result: " .. (err or "unknown error")
	end

	return res
end

-- UPDATE 操作
function M.update(params)
	local db, err = get_db()
	if not db then
		return nil, err
	end

	local table_name = params.table
	local set_clause = {}
	local set_values = {}

	for k, v in pairs(params.data) do
		table.insert(set_clause, k .. " = ?")
		table.insert(set_values, v)
	end

	local where_clause, where_values = build_where_clause(params.conditions)

	local sql = string.format("UPDATE %s SET %s%s", table_name, table.concat(set_clause, ", "), where_clause)

	local values = {}
	for _, v in ipairs(set_values) do
		table.insert(values, v)
	end
	for _, v in ipairs(where_values) do
		table.insert(values, v)
	end

	local res, err, errno, sqlstate = db:query(sql, values)
	db:set_keepalive(10000, 100)

	if not res then
		return nil, "bad result: " .. (err or "unknown error")
	end

	return res
end

-- DELETE 操作
function M.delete(params)
	local db, err = get_db()
	if not db then
		return nil, err
	end

	local table_name = params.table
	local where_clause, where_values = build_where_clause(params.conditions)

	local sql = string.format("DELETE FROM %s%s", table_name, where_clause)

	local res, err, errno, sqlstate = db:query(sql, where_values)
	db:set_keepalive(10000, 100)

	if not res then
		return nil, "bad result: " .. (err or "unknown error")
	end

	return res
end

local function example()
	-- SELECT 示例with various conditions
	local select_result, err = M.select({
		table = "users",
		fields = "id, name, email",
		conditions = {
			age = { ">=", 30 },
			name = { "LIKE", "%John%" },
			status = { "IN", { "active", "pending" } },
			created_at = { "BETWEEN", { "2023-01-01", "2023-12-31" } },
			is_admin = true, -- 等值条件
		},
		order = "id DESC",
		limit = 10,
	})
end

return M
