local _M = {}
local map = require("config.keymaps")

function _M.alpha_config()
    local dashboard = require("alpha.themes.dashboard")
    local logo = [[
███╗   ███╗ ██████╗ ███╗   ██╗ ██████╗ ██╗ █████╗
████╗ ████║██╔═══██╗████╗  ██║██╔════╝ ██║██╔══██╗
██╔████╔██║██║   ██║██╔██╗ ██║██║  ███╗██║███████║
██║╚██╔╝██║██║   ██║██║╚██╗██║██║   ██║██║██╔══██║
██║ ╚═╝ ██║╚██████╔╝██║ ╚████║╚██████╔╝██║██║  ██║
╚═╝     ╚═╝ ╚═════╝ ╚═╝  ╚═══╝ ╚═════╝ ╚═╝╚═╝  ╚═╝
      ]]
    dashboard.section.header.val = vim.split(logo, "\n")
    require("alpha").setup(dashboard.config)
end

function _M.gutentags_config()
end

function _M.whichkey_config()
    vim.o.timeout = true
    vim.o.timeoutlen = 300

    local wk = require("which-key")
    wk.register(map.common_map())

    local opt = {
        layout = {
            height = { min = 2, max = 25 }, -- min and max height of the columns
            width = { min = 20, max = 50 }, -- min and max width of the columns
            spacing = 3,                    -- spacing between columns
            align = "center",               -- align columns left, center or right
        },
        window = {
            border = "none",  -- none, single, double, shadow
            position = "bottom", -- bottom, top
            margin = { 0, 0, 0, 0 }, -- extra window margin [top, right, bottom, left]. When between 0 and 1, will be treated as a percentage of the screen size.
            padding = { 1, 2, 1, 2 }, -- extra window padding [top, right, bottom, left]
            winblend = 100,     -- value between 0-100 0 for fully opaque and 100 for fully transparent
            zindex = 1000,    -- positive value to position WhichKey above other floating windows.
        },
    }
    wk.setup(opt)
end

function _M.symbols_outline_config()
    local opt = {
        relative_width = false,
        width = 30,
    }
    require("symbols-outline").setup(opt)
end

function _M.neotree_config()
    local opt = {
        close_if_last_window = true,
        window = {
            width = 30,
        },
    }
    require("neo-tree").setup(opt)
end

function _M.mason_config()
    local opt = {
        ui = {
            icons = {
                package_installed = "✓",
                package_pending = "➜",
                package_uninstalled = "✗",
            },
        },
        ensure_installed = {
            "stylua",
            "codelldb",
            "shfmt",
            "delve",
            "clangd",
        },
    }
    require("mason").setup(opt)
end

function _M.lsp_config()
    local nvim_lsp = require("lspconfig")
    require("mason").setup()
    require("mason-lspconfig").setup()

    local opt = {
        autostart = true
    }

    nvim_lsp.clangd.setup(opt)
    nvim_lsp.gopls.setup(opt)
    nvim_lsp.lua_ls.setup(opt)
end

function _M.mason_lsp_config()
    require("mason-lspconfig").setup()
end

--function _M.null_ls_config()
--	local null_ls = require("null-ls")
--	local opt = {
--		sources = {
--			null_ls.builtins.formatting.stylua,
--			null_ls.builtins.diagnostics.eslint,
--			null_ls.builtins.completion.spell,
--		},
--	}
--
--	null_ls.setup(opt)
--end

function _M.scheme_config()
end

function _M.cmp_config()
    local cmp = require("cmp")
    local lspkind = require("lspkind")
    local opt = {
        completion = {
            completeopt = "menu,menuone,noinsert",
        },
        snippet = {
            expand = function(args)
                require("luasnip").lsp_expand(args.body)
            end,
        },
        mapping = cmp.mapping.preset.insert({
            ["<C-n>"] = cmp.mapping.select_next_item({ behavior = cmp.SelectBehavior.Insert }),
            ["<C-p>"] = cmp.mapping.select_prev_item({ behavior = cmp.SelectBehavior.Insert }),
            ["<C-b>"] = cmp.mapping.scroll_docs(-4),
            ["<C-f>"] = cmp.mapping.scroll_docs(4),
            ["<C-Space>"] = cmp.mapping.complete(),
            ["<C-e>"] = cmp.mapping.abort(),
            ["<CR>"] = cmp.mapping.confirm({ select = true }), -- Accept currently selected item. Set `select` to `false` to only confirm explicitly selected items.
            ["<S-CR>"] = cmp.mapping.confirm({
                behavior = cmp.ConfirmBehavior.Replace,
                select = true,
            }), -- Accept currently selected item. Set `select` to `false` to only confirm explicitly selected items.
        }),
        sources = cmp.config.sources({
            { name = "nvim_lsp" },
            { name = "luasnip" },
            { name = "buffer" },
            { name = "path" },
        }),
        -- 显示提示图标
        formatting = {
            format = lspkind.cmp_format({
                with_text = true, -- do not show text alongside icons
                maxwidth = 50,    -- prevent the popup from showing more than provided characters (e.g 50 will not show more than 50 characters)
                before = function(entry, vim_item)
                    -- Source 显示提示来源
                    vim_item.menu = "[" .. string.upper(entry.source.name) .. "]"
                    return vim_item
                end
            })
        },
    }
    cmp.setup(opt)
end

function _M.treesitter_config()
    local opt = {
        -- A list of parser names, or "all" (the five listed parsers should always be installed)
        ensure_installed = { "c", "lua", "vim", "vimdoc", "query", "javascript", "go", "cpp", "proto", "typescript",
            "markdown" },

        -- Install parsers synchronously (only applied to `ensure_installed`)
        sync_install = false,

        -- Automatically install missing parsers when entering buffer
        -- Recommendation: set to false if you don't have `tree-sitter` CLI installed locally
        auto_install = true,

        -- List of parsers to ignore installing (or "all")
        ignore_install = {},

        ---- If you need to change the installation directory of the parsers (see -> Advanced Setup)
        -- parser_install_dir = "/some/path/to/store/parsers", -- Remember to run vim.opt.runtimepath:append("/some/path/to/store/parsers")!

        highlight = {
            enable = true,

            -- NOTE: these are the names of the parsers and not the filetype. (for example if you want to
            -- disable highlighting for the `tex` filetype, you need to include `latex` in this list as this is
            -- the name of the parser)
            -- list of language that will be disabled
            disable = { "c", "rust" },
            -- Or use a function for more flexibility, e.g. to disable slow treesitter highlight for large files
            disable = function(lang, buf)
                local max_filesize = 100 * 1024 -- 100 KB
                local ok, stats = pcall(vim.loop.fs_stat, vim.api.nvim_buf_get_name(buf))
                if ok and stats and stats.size > max_filesize then
                    return true
                end
            end,

            -- Setting this to true will run `:h syntax` and tree-sitter at the same time.
            -- Set this to `true` if you depend on 'syntax' being enabled (like for indentation).
            -- Using this option may slow down your editor, and you may see some duplicate highlights.
            -- Instead of true it can also be a list of languages
            additional_vim_regex_highlighting = false,
        },
    }

    require("nvim-treesitter.configs").setup(opt)
end

return _M
