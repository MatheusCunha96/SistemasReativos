  function naimagem (mx, my, x, y, w, h) 
    return (mx>x) and (mx<x+w) and (my>y) and (my<y+h)
  end  
  
function retangulo (x,y,w,h)
    local orig_x, orig_y, rx, ry, rw, rh = x, y, x, y, w, h
    
    return {
      
      draw = 
        function ()
            love.graphics.rectangle("line", rx, ry, rw, rh)
        end,
      
      keypressed =
        function (key)
            local mx, my = love.mouse.getPosition() 
            if key == 'b' and naimagem (mx,my, rx, ry, rw, rh) then
               ry = 200
            end
            if key == 'down' and naimagem(mx,my, rx, ry, rw, rh)  then
              ry = ry + 10
            end
            if key == 'right' and naimagem(mx,my, rx, ry, rw, rh)  then
              rx = rx + 10
            end
        end
      
    }
end

function love.load()
  ret_array = {
    retangulo(50, 100, 100, 100),
    retangulo(100, 210, 100, 100),
    retangulo(150, 320, 100, 100),
    retangulo(200, 430, 100, 100)
    }
  
end

function love.keypressed(key)
  
  for i = 1, #ret_array do
    ret_array[i].keypressed(key)
  end
  
end


function love.update (dt)
  
end

function love.draw ()
  
  for i = 1, #ret_array do
    ret_array[i].draw()
  end
  
end


