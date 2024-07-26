local _M = {
	SUCCESS = { code = 0, msg = "success" },

	INTERNAL_SERVER_ERROR = { code = 50001, msg = "internal error" },

	MODULE_NOT_FOUND = { code = 10001, msg = "module not found" },
	METHOD_NOT_FOUND = { code = 10002, msg = "method not found" },

	PARAM_ERROR = { code = 20001, msg = "param check failed" },
}

local mt = {
	__index = function(t, k)
		error("undefined code: " .. tostring(k), 2)
	end,
}

setmetatable(_M, mt)

return _M
