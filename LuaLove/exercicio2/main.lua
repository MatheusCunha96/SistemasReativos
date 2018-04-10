
function newblip (espera)
  local x, y = 0, 0
  return {
    
    vivo = true,
    tempo_antigo=0,
    tempo_espera = 0,
    
    update = coroutine.wrap (function (self)
      local width, height = love.graphics.getDimensions( )
      while true do
        x = x+3
        if x > width then
        -- volta para a esquerda da janela
          x = 0
        end
        wait(espera/100, self)
      end
    end),
  
    affected = function (pos)
      if pos>x and pos<x+10 then
      -- "pegou" o blip
        return true
      else
        return false
      end
    end,
    draw = function ()
      love.graphics.rectangle("line", x, y, 10, 10)
    end
  }
end

function wait(segundos, meublip)
  
  meublip.vivo = false
  meublip.tempo_espera = segundos
  coroutine.yield()
  
end

function newplayer ()
  local x, y = 0, 200
  local width, height = love.graphics.getDimensions( )
  return {
  try = function ()
    return x
  end,
  update = function (dt)
    x = x + 0.5
    if x > width then
      x = 0
    end
  end,
  draw = function ()
    love.graphics.rectangle("line", x, y, 30, 10)
  end
  }
end


function love.keypressed(key)
  if key == 'a' then
    pos = player.try()
    for i in ipairs(listabls) do
      local hit = listabls[i].affected(pos)
      if hit then
        table.remove(listabls, i) -- esse blip "morre" 
        return -- assumo que apenas um blip morre
      end
    end
  end
end


function love.load()
  player =  newplayer()
  listabls = {}
  for i = 1, 5 do
    listabls[i] = newblip(i)
  end
end


function love.draw()
  player.draw()
  for i = 1,#listabls do
    listabls[i].draw()
    love.graphics.print(tostring(listabls[i].tempo_espera), 0, 30*i)
  end  
end

function love.update(dt)
  tempo = love.timer.getTime()
  player.update(dt)
  
  for i = 1,#listabls do
    
    if tempo - listabls[i].tempo_antigo >=  listabls[i].tempo_espera then
      listabls[i].vivo = true  
      listabls[i].tempo_antigo = tempo
    end
    
    if listabls[i].vivo then
      listabls[i]:update()
    end
    
  end  
  
end
  