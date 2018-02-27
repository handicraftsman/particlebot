http = require 'http.request'

on('PRIVMSGEvent', function(e)
  local reply_to
  if e.target == get_nick(e.socket) then
    reply_to = e.nick
  else
    reply_to = e.target
  end
  local url = string.match(e.message, '(https?://[%d%w%.%/%%%&%?]+%.[%d%w%.%/%%%&%?]+)')
  if url ~= nil then
    local headers, stream = http.new_from_uri(url):go()
    if headers == nil then
      privmsg(e.socket, reply_to, e.nick .. ' -> Unable to get ' .. url .. '!')
      return
    end
    local body = stream:get_body_as_string()
    if body == nil then
      privmsg(e.socket, reply_to, e.nick .. ' -> Unable to get body for ' .. url .. '!')
    end
    if headers:get(':status') == '200' then
      local title = string.match(body, [[<title>([^<]+)</title>]])
      if title ~= nil then
        privmsg(e.socket, reply_to, e.nick .. ' -> ' .. title:gsub("&nbsp;", ""):gsub("&quot;", ""))
      else
        privmsg(e.socket, reply_to, e.nick .. ' -> ' .. headers:get(':status') .. ' ' .. headers:get('content-type'))
      end
    else
      privmsg(e.socket, reply_to, e.nick .. ' -> ' .. headers:get(':status'))
    end
  end
end)