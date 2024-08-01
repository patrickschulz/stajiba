#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void _engineering_notation(double number, double* new_num, char* prefix)
{
    if(number == 0)
    {
        *new_num = 0;
        *prefix = ' ';
        return;
    }
    char powertable[] = {
        'y', // yocto
        'z', // zepta
        'a', // atto
        'f', // femto
        'p', // pico
        'n', // nano
        'u', // micro
        'm', // milli
        ' ',  // - none -
        'k', // kilo
        'M', // Mega
        'G', // Giga
        'T', // Tera
        'P', // Peta
        'E', // Exa
        'Z', // Zetta
        'Y', // Yotta
    };
    int power = floor(log(fabs(number)) / log(10) / 3);
    *new_num = number / pow(10, 3 * power);
    *prefix = powertable[power + 8]; // 8: offset to zero power
}

char* engineering_format(double number, const char* unit, int precision)
{
    double new_num;
    char prefix;
    _engineering_notation(number, &new_num, &prefix);
    char* fmt = malloc(100); // FIXME
    sprintf(fmt, "%%%d.%df %%c%%s", 3 + 1 + precision, precision);
    char* result = malloc(100); // FIXME
    sprintf(result, fmt, new_num, prefix, unit);
    free(fmt);
    return result;
}

/*
function parse_number_unit(str)
    local prefixtable = { 'y', 'z', 'a', 'f', 'p', 'n', 'u', 'm', ' ', 'k', 'M', 'G', 'T', 'P', 'E', 'Z', 'Y' }

    local factortable = {}
    for i, prefix in ipairs(prefixtable) do
        factortable[prefix] = -27 + 3 * i
    end

    local numbermatch = "[%d.]+"
    local prefixmatch = string.format("[%s]", table.concat(prefixtable))
    local unitmatch = "."
    local matchstr = string.format("(%s)(%s)(%s)", numbermatch, prefixmatch, unitmatch)
    local number, prefix, unit = string.match(str, matchstr)
    return tonumber(number), math.pow(10, factortable[prefix]), unit
end
*/
