local opt = vim.opt

opt.syntax = "ON"            --语法高亮
opt.termguicolors = true -- 开启真彩色

-- 设置文件读取格式，终端格式等等
opt.fencs = "utf-8, gbk"
opt.termencoding = "utf-8"
opt.fileencoding = "utf-8"
opt.encoding = "utf-8"

-- 关闭兼容模式
opt.compatible = false
-- 输入查找的同时，立刻进行匹配并显示匹配的位置
opt.incsearch = true
-- 查找忽略大小写
opt.ignorecase = true
-- 取消tab选择
opt.wildmenu = false
-- 当检测到一个文件已经在Vim之外被更改，并且它没有在Vim的内部被更改时，自动读取它
opt.autoread = true
-- 设置光标不要闪烁
opt.gcr = "a:block-blinkon0"
-- 将状态行显示在窗口底部倒数第二行
opt.laststatus = 2
-- 底部的行号等显示
opt.ruler = true
-- 行号显示(nu)
opt.number = true
-- 高亮当前行(青色)
opt.cursorline = true
-- 查询显示高亮
opt.hlsearch = true
-- 使vi可以用退格键删除
opt.backspace = "indent,eol,start"
-- 是vim不闪屏
--opt.novisualbell = true
-- 自动缩进时缩进为4格(sw)
opt.shiftwidth = 4
-- 启动折叠
-- opt.foldenable
-- 启用语法折叠
-- opt.foldmethod=syntax
opt.foldenable = false
-- 语言设置
opt.langmenu = "zn_CN.UTF-8"
opt.helplang = "cn"
-- 高亮显示括号
opt.showmatch = true
opt.matchtime = 5
--  不换行
opt.wrap = false
-- #开启自动缩进   (ai)
opt.autoindent = true
-- #编辑时可以将tab替换为空格(et)，输入的不再是tab，而是空格
opt.expandtab = true
-- 设置tab等于4个空格
opt.tabstop = 4
-- 设置右下角显示完整命令
-- opt.showcmd
-- 设置c语言自动缩进
opt.cindent = true
-- 设置鼠标可以在普通模式跟可视模式使用
opt.mouse = "nv"
-- 设置tag索引文件查询目录，当使用gentags时可以去掉该选项
opt.tags="tags,../tags,../../tags,../../../../tags"
-- 设置path路径，方便查询系统函数及库函数
opt.path = ".,/usr/include,/usr/local/include,/usr/include/c++/4.4.7,/usr/include/c++/4.4.4"
-- 设置modifiable
opt.modifiable = true

vim.cmd.colorscheme "monokai-pro-octagon"  -- 主题色
