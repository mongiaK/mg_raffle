local cfg = require("config.plugins_config")
local map = require("config.keymaps")

return {
    {
        -- 自动生成tags文件
        "ludovicchabant/vim-gutentags",
        enabled = true,
        config = cfg.gutentags_config
    },
    {
        -- 提示显示快捷键
        "folke/which-key.nvim",
        event = "VeryLazy",
        config = cfg.whichkey_config
    },
    {
        -- 左侧窗口显示目录树
        "nvim-neo-tree/neo-tree.nvim",
	    dependencies = {
            "nvim-lua/plenary.nvim",
            "nvim-tree/nvim-web-devicons", -- not strictly required, but recommended
            "MunifTanjim/nui.nvim",
        },
        config = cfg.neotree_config,
        keys = map.neotree_map(),
    },
}
