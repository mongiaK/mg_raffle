local cfg = require("config.plugins_config")
local map = require("config.keymaps")

return {
    {
        -- 配置lsp客户端，不同的语言都要单独配置，比较麻烦
        "neovim/nvim-lspconfig",
        config = cfg.lsp_config,
        event = { "BufReadPre", "BufNewFile" },
	    dependencies = {
		   "williamboman/mason.nvim", -- lsp server install manager
		   "williamboman/mason-lspconfig.nvim",  --lsp server config
	    },
        keys = map.lsp_map(),
    },
    {
        -- 窗口右侧显示函数，全局变量
        "simrat39/symbols-outline.nvim",
        event = "VeryLazy",
        config = cfg.symbols_outline_config,
        keys = map.symbols_outline_map(),
    },
    {
        -- 丰富nvim 内置lsp的函数，显示更好看
        "nvimdev/lspsaga.nvim",
        config = function()
            require('lspsaga').setup({})
        end,
        dependencies = {
            "nvim-treesitter/nvim-treesitter",
            "nvim-tree/nvim-web-devicons",
        }
    },
    {
        -- 语法高亮
        "nvim-treesitter/nvim-treesitter",
        config = cfg.treesitter_config,
        lazy= true,
    },
    {
        --自动匹配括号之类的
        "echasnovski/mini.pairs",
        event = "VeryLazy",
        config = function ()
            require('mini.pairs').setup()
        end
    },
    {
        -- 包含符号处理
        "echasnovski/mini.surround",
        event = "VeryLazy",
        config = function ()
            require('mini.surround').setup()
        end
    },
    {
        -- 添加注释
        "echasnovski/mini.comment",
        event = "VeryLazy",
        config = function ()
            require("mini.comment").setup()
        end
    }
}
