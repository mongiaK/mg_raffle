local _M = {}

function _M.common_map()
    return {
       ["<leader>l"] = { name = "+lsp" },
       ["<leader>L"] = { "<cmd>Lazy<cr>", "Lazy"}
    }
end

function _M.neotree_map()
    return {
        { "<leader>n", "<cmd>Neotree toggle<cr>", desc = "NeoTree toggle" },
    }
end

function _M.mason_map()
    return {
        { "<leader>m", "<cmd>Mason<cr>", desc = "Mason"}
    }
end

function _M.lsp_map()
    return {
        {"<leader>li", "<cmd>LspInfo<cr>", desc = "LspInfo"},
        {"<leader>lf", "<cmd>lua vim.lsp.buf.format()<cr>", desc = "Lsp Format"},
    }
end

function _M.symbols_outline_map()
    return {
        {"<leader>t", "<cmd>SymbolsOutline<cr>", desc = "show function"}
    }
end

return _M
