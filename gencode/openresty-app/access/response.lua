local _M = {}
local cjson = require("cjson")

function _M.say(data)
	ngx.say(cjson.encode(data))
end

return _M
