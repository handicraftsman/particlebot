function eval(str)
  local f = loadstring('return ' .. str)
  if f then
    local locals = {}
    local caller = debug.getinfo(2, 'f').func
    local i, k, v = 2, debug.getupvalue(caller, 1)
    while k do
      locals[k] = v
      i, k, v = i+1, debug.getupvalue(caller, i)
    end
    i, k, v = 2, debug.getlocal(2, 1)
    while k do
      locals[k] = v
      i, k, v = i+1, debug.getlocal(2, i)
    end
    local fenv = debug.getfenv(caller)
    setmetatable(locals, { __index = fenv })
    setfenv(f, locals)
    return f()
  end
end

command({
  name = 'eval',
  group = 'eval',
  description = 'Evaluates given string',
  usage = '[text...]',
  level = 4
}, function(e)

  local str = ''
  for _, s in ipairs(e['split']) do
    str = str .. s .. ' '
  end

  local ret = { eval(str) }
  
  for i, r in ipairs(ret) do
    reply(e, '&B#' .. i .. ':&N ' .. require('pl.pretty').write(r))
  end
end)