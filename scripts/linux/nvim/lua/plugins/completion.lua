local cfg = require("config.plugins_config")

return {
    {
        "hrsh7th/nvim-cmp",
         ersion = false, -- last release is way too old
         event = "InsertEnter",
         config = cfg.cmp_config,
         dependencies = {
             "hrsh7th/cmp-nvim-lsp",
             "hrsh7th/cmp-buffer",
             "hrsh7th/cmp-path",
             "hrsh7th/cmp-cmdline",
             
             -- 代码补全引擎
             "saadparwaiz1/cmp_luasnip",
             "L3MON4D3/LuaSnip",

             -- 包含了发部分常用语言的代码段
             "rafamadriz/friendly-snippets",
             -- 显示分类的小图标支持
             "onsails/lspkind-nvim",
         },
    }
}
