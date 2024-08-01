local transfer = require "modules/transfer"

local M = {}

local function _get_indentstr(document)
    return string.rep("    ", document.envcounter)
end

local function _increase_indent(document)
    document.envcounter = document.envcounter + 1
end

local function _decrease_indent(document)
    document.envcounter = document.envcounter - 1
end

local function _insert(document, str)
    table.insert(document.content, string.format("%s%s", _get_indentstr(document), str))
end

local function _insert_lines(document, lines)
    for _, line in ipairs(lines) do
        table.insert(document.content, string.format("%s%s", _get_indentstr(document), line))
    end
end

local function _add_comma(t)
    for i = 1, #t do
        t[i] = t[i] .. ","
    end
end

function M.create_document(filename)
    local document = {
        filename = filename,
        content = {},
        temp = {},
        envcounter = 0
    }
    local packages = {
        { name = "contour", opt = { "outline" } },
        { name = "pgfplots" },
        { name = "siunitx" },
        { name = "booktabs" },
    }
    _insert(document, "\\documentclass[paper = a4, landscape, headinclude = false, footinclude = false, 10pt]{scrartcl}")
    _insert(document, "\\areaset{27cm}{20cm}")
    for _, package in ipairs(packages) do
        if package.opt then
            _insert(document, string.format("\\usepackage[%s]{%s}", table.concat(package.opt), package.name))
        else
            _insert(document, string.format("\\usepackage{%s}", package.name))
        end
    end
    local extracommands = {
        "pagestyle{empty}",
    }
    for _, cmd in ipairs(extracommands) do
        _insert(document, string.format("\\%s", cmd))
    end
    -- taken from: https://sashamaps.net/docs/resources/20-colors/
    local colordefs = {
        { r =   0, g = 130, b = 200 }, -- blue
        { r = 230, g =  25, b =  75 }, -- red
        { r =  60, g = 180, b =  75 }, -- green
        { r = 255, g = 225, b =  25 }, -- yellow
        { r = 245, g = 130, b =  48 }, -- orange
        { r = 145, g =  30, b = 180 }, -- purple
        { r =  70, g = 240, b = 240 }, -- cyan
        { r = 240, g =  50, b = 230 }, -- pink
        { r = 210, g = 245, b =  60 }, -- light green
        { r = 250, g = 190, b = 212 }, -- pig pink
        { r   = 0, g = 128, b = 128 }, -- teal
        { r = 220, g = 190, b = 255 }, -- light purple
        { r = 170, g = 110, b =  40 }, -- ochre
        { r = 255, g = 250, b = 200 }, -- light yellow
        { r = 128, g =   0, b =   0 }, -- brown
        { r = 170, g = 255, b = 195 }, -- light turqouise
        { r = 128, g = 128, b =   0 }, -- slime green
        { r = 255, g = 215, b = 180 }, -- beige
        { r =   0, g =   0, b = 128 }, -- dark blue
        { r = 128, g = 128, b = 128 }, -- grey
        { r = 255, g = 255, b = 255 }, -- white
        { r =   0, g =   0, b =   0 }, -- black
    }
    for i, def in ipairs(colordefs) do
        _insert(document, string.format("\\definecolor{plotcolor%d}{RGB}{%d,%d,%d}", i, def.r, def.g, def.b))
    end
    local pgfplotopts = {
        "compat=1.18",
        "compat/show suggested version=false",
        "scale only axis = true",
        "scaled ticks=false",
        "height = 5cm",
        "axis line style = {thick}",
        "tick align = inside",
        "major tick length = {4pt}",
        "minor tick length = {2pt}",
        "every tick/.append style = {semithick, black}",
        "every axis plot/.style={very thick, line join = round}",
        "no markers",
        "grid=both",
        "grid style={black, line width = 0.4pt, dash pattern = on 0pt off 1.0pt, line cap = round}",
        "enlarge x limits = false",
        "enlarge y limits = false",
        "legend cell align=left",
        "legend style = {draw = none, at = {(0.5, -0.0)}, yshift = -1.2cm, anchor = north}",
        "max space between ticks = 30",
    }
    local cyclelist = {
        "plotcolor1",
        "plotcolor2",
        "plotcolor3",
        "plotcolor4",
        "plotcolor5",
        "plotcolor6",
        "plotcolor7",
        "plotcolor8",
        "plotcolor9",
        "plotcolor10",
        "plotcolor11",
        "plotcolor12",
        "plotcolor13",
        "plotcolor14",
        "plotcolor15",
        "plotcolor16",
        "plotcolor17",
        "plotcolor18",
        "plotcolor19",
        "plotcolor20",
        "plotcolor21",
        "plotcolor22",
        "plotcolor23",
    }
    table.insert(pgfplotopts, string.format("cycle list = {%s}", table.concat(cyclelist, ", ")))
    _insert(document, "\\pgfplotsset{")
    _increase_indent(document)
    _add_comma(pgfplotopts)
    _insert_lines(document, pgfplotopts)
    _decrease_indent(document)
    _insert(document, "}")
    return document
end

function M.new_paragraph(document, str)
    _insert(document, "\\par")
end

function M.paragraph(document, str)
    M.new_paragraph(document)
    _insert(document, str)
    M.new_paragraph(document)
end

function M.start_environment(document, env, opt)
    if opt and #opt > 0 then
        print(env)
        _insert(document, string.format("\\begin[%s]{%s}", table.concat(opt, ", "), env))
    else
        _insert(document, string.format("\\begin{%s}", env))
    end
    _increase_indent(document)
end

function M.finish_environment(document, env)
    _decrease_indent(document)
    _insert(document, string.format("\\end{%s}", env))
end

function M.start_axis(document, opt)
    M.start_environment(document, "tikzpicture")
    _insert(document, "[baseline = (plot.north)]")
    M.start_environment(document, "axis")
    local baseopt = {
        "name = plot"
    }
    if opt.title then
        table.insert(baseopt, string.format("title = %s", opt.title))
    end
    local optstr = table.concat(baseopt, ", ")
    if opt then
        optstr = string.format("%s, %s", optstr, table.concat(opt, ", "))
    end
    _insert(document, string.format("[%s]", optstr))
end

function M.insert_plot(document, x, y, legend, format)
    if #x ~= #y then
        error(string.format("tex.insert_plot: x and y must have the same dimension (%d != %d)", #x, #y))
    end
    if format then
        _insert(document, string.format("\\addplot+[smooth, %s] coordinates {", table.concat(format, ", ")))
        --_insert(document, string.format("\\addplot+[%s] coordinates {", table.concat(format, ", ")))
    else
        _insert(document, "\\addplot+[smooth] coordinates {")
        --_insert(document, "\\addplot coordinates {")
    end
    _increase_indent(document)
    for i = 1, #x do
        _insert(document, string.format("(%s, %s)", tostring(x[i]), tostring(y[i])))
    end
    _decrease_indent(document)
    _insert(document, "};")
    if legend then
        _insert(document, string.format("\\addlegendentry{%s}", legend))
    end
end

function M.add_plot_node(document, x, y, text)
    _insert(document, string.format("\\node at (%f, %f) {\\contour{white}{%s}};", x, y, text))
end

function M.insert_axis_horizontal_line(document, x)
    _insert(document, string.format("\\draw ({axis cs:%s, 0} |- {rel axis cs:0, 0}) -- ({axis cs:%s, 0} |- {rel axis cs:0, 1});", tostring(x), tostring(x)))
end

function M.finish_axis(document)
    M.finish_environment(document, "axis")
    M.finish_environment(document, "tikzpicture")
end

function M.start_bode_diagram(document, opt)
    document.temp.bode = {}
    document.temp.bode.opt = opt
end

function M.insert_bode_plot(document, x, y, legend, format)
    local bode = document.temp.bode
    if #x ~= #y then
        error(string.format("tex.insert_plot: x and y must have the same dimension (%d != %d)", #x, #y))
    end
    bode.xdata = bode.xdata or {}
    bode.ydata = bode.ydata or {}
    bode.label = bode.label or {}
    table.insert(bode.xdata, x)
    table.insert(bode.ydata, y)
    table.insert(bode.label, legend)
    -- calculate phase boundaries
    local phi = transfer.phase(y)
    for i = 1, #y do
        bode.ymax = math.max(bode.ymax or -math.huge, phi[i])
        bode.ymin = math.min(bode.ymin or math.huge, phi[i])
    end
    -- round phase boundaries to multiples of 45
    bode.ymin = make_multiple_floor(bode.ymin, 45)
    bode.ymax = make_multiple_floor(bode.ymax, 45)
end

function M.finish_bode_diagram(document)
    local bode = document.temp.bode
    M.start_environment(document, "tikzpicture")
    _insert(document, "[baseline = (magnitude.north)]")

    local totalheight = 5
    local gap = 0.2
    local singleheight = (totalheight - gap) / 2

    M.start_environment(document, "axis")
    local magnitudebaseopt = {
        "name = magnitude",
        string.format("height = %.4fcm", singleheight),
        "anchor = south",
        "xmode = log",
        "xticklabels = {}",
        "ylabel = {$|H^2(s)|$ (\\unit{\\deci\\bel})}",
    }
    if bode.opt.title then
        table.insert(magnitudebaseopt, string.format("title = %s", bode.opt.title))
    end
    if bode.opt then
        _insert(document, string.format("[%s, %s]", table.concat(magnitudebaseopt, ", "), table.concat(bode.opt, ", ")))
    else
        _insert(document, string.format("[%s]", table.concat(magnitudebaseopt, ", ")))
    end
    for i = 1, #bode.xdata do
        local x = bode.xdata[i]
        local y = transfer.dB20(bode.ydata[i])
        M.insert_plot(document, x, y)
    end
    M.finish_environment(document, "axis")

    M.start_environment(document, "axis")
    local phasebaseopt = {
        "name = phase",
        string.format("height = %.4fcm", singleheight),
        "anchor = north",
        string.format("yshift = -%.4fcm", gap),
        "xmode = log",
        "xlabel = {$f$ (\\unit{\\hertz})}",
        "ylabel = {$\\varphi$ (\\unit{\\degree})}",
        string.format("ymin = %.0f", bode.ymin),
        string.format("ymax = %.0f", bode.ymax),
        string.format("ytick = {%.0f, %.0f, ..., %.0f}", bode.ymin, bode.ymin + 45, bode.ymax)
    }
    if bode.opt then
        _insert(document, string.format("[%s, %s]", table.concat(phasebaseopt, ", "), table.concat(bode.opt, ", ")))
    else
        _insert(document, string.format("[%s]", table.concat(phasebaseopt, ", ")))
    end
    for i = 1, #bode.xdata do
        local x = bode.xdata[i]
        local y = transfer.phase(bode.ydata[i])
        local label = bode.label[i]
        M.insert_plot(document, x, y, label)
    end
    M.finish_environment(document, "axis")

    M.finish_environment(document, "tikzpicture")

    document.temp.bode = nil
end

function M.start_table(document, columspec)
    M.start_environment(document, "tabular")
    _insert(document, string.format("{%s}", columspec))
    _insert(document, "\\toprule")
end

function M.insert_table_row(document, ...)
    local columns = { ... }
    _insert(document, string.format("%s \\\\", table.concat(columns, " & ")))
end

function M.finish_table(document)
    _insert(document, "\\bottomrule")
    M.finish_environment(document, "tabular")
end

function M.hfill(document)
    _insert(document, "\\hfill")
end

function M.vfill(document)
    _insert(document, "\\vfill")
end

function M.insert_primitive(document, str)
    _insert(document, str)
end

function M.start_document(document)
    M.start_environment(document, "document")
end

function M.finish_document(document)
    M.finish_environment(document, "document")
end

function M.compile_document(document)
    local file = io.open(string.format("%s.tex", document.filename), "w")
    file:write(table.concat(document.content, "\n"))
    file:close()
    os.execute(string.format("pdflatex %s.tex", document.filename))
end

function M.cleanup(document)
    os.remove(string.format("%s.tex", document.filename))
    os.remove(string.format("%s.log", document.filename))
    os.remove(string.format("%s.aux", document.filename))
end

return M
