local cfg = require("config.plugins_config")

return {
    {
        -- neovim 初始打开ui显示
        "goolord/alpha-nvim",
        event = "VimEnter",
        dependencies = { 'nvim-tree/nvim-web-devicons' },
        config = cfg.alpha_config
    },
    {
        -- 打开一个终端页面
        "kassio/neoterm",
        event = "VeryLazy",
    },
    {
        -- 配色
         "catppuccin/nvim",
         name = "catppuccin",
         priority = 1000
    },
    {
        -- 配色
        "lockvl842/monokai-pro.nvim",
        config = cfg.scheme_config,
        priority = 1000
    },

}
