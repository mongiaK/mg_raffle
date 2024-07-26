local router = require("access.router")

local function main()
	local segments = {}
	for segment in string.gmatch(ngx.var.uri, "([^/]+)") do
		table.insert(segments, segment)
	end

	local module_name = segments[2]
	local func_name = segments[3]

	router.route(module_name, func_name)
end

main()
