local resp = require("access.response")
local code = require("common.code")
local cjson = require("cjson")
local _M = {}

_M.modules = {
	example = require("core.example"),
}

function _M.get_json_params()
	local method = ngx.req.get_method()
	local params = {}
	if method == "GET" then
		local args = ngx.req.get_uri_args()
		params = args
	elseif method == "POST" then
		ngx.req.read_body()
		local body = ngx.req.get_body_data()
		if body then
			pcall(function()
				params = cjson.decode(body)
			end)
		end
	end

	return params
end

function _M.route(module_name, func_name)
	local module = _M.modules[module_name]
	if module then
		local func = module[func_name]
		if type(func) == "function" then
			local params = _M.get_json_params()
			local ok, result = pcall(func, params)
			if ok then
				return resp.say(result)
			else
				return resp.say(code.INTERNAL_SERVER_ERROR)
			end
		else
			return resp.say(code.METHOD_NOT_FOUND)
		end
	end
	return resp.say(code.MODULE_NOT_FOUND)
end

return _M
