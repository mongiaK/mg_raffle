local _M = {}
local code = require("common.code")

function _M.helloworld(params)
	if _M.check_param(params) then
		return code.PARAM_ERROR
	end

	local ret = _M.run()

	return _M.done(ret)
end

function _M.run()
	return {
		hello = "world",
	}
end

function _M.check_param(params)
	return nil
end

function _M.done(data)
	local success = code.SUCCESS
	success.data = data
	return success
end

return _M
