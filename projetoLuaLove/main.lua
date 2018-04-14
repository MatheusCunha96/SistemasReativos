
function new_player(x, y, r)
  local posx, posy, radius = x, y, r
  
  
  return {
    
    dtMult = 200,
    
    draw = function ()      
            love.graphics.circle('fill', posx, posy, radius)
    end,
   
    
    keyDown = function (self, dt)
      
      if love.keyboard.isDown("down") then
        posy = posy + self.dtMult*dt
      end
      if love.keyboard.isDown("up") then
        posy = posy - self.dtMult*dt
      end
      if love.keyboard.isDown("right") then
        posx = posx + self.dtMult*dt
      end
      if love.keyboard.isDown("left") then
        posx = posx - self.dtMult*dt
      end
      
      
    end
    
  
   
    
  
  
  }
end



function new_enemy(x, y, w, h)
   
  return {
    
  posx = x, 
  posy = y,
  width = w,
  height = h,
    
   draw = function (self)
          love.graphics.rectangle('fill', self.posx, self.posy, self.width, self.height)
          love.graphics.rectangle('fill', self.width + 50, self.posy, 800 - self.width, self.height)
    end,
    
   randomWidth = function (self)
      self.width = math.random(0,350)
   end,
    
   update = function (self, dt)
      
      self.posy = self.posy + 400*dt
      
      if self.posy >= 600 then
        self:randomWidth()
        self.posy = 0 - self.height
      end
      
    end  
    
  }

end


function love.load()
   
  player = new_player(405, 500, 10)
  enemy_array = {new_enemy(0, 200, math.random(0,800), 50),
                 new_enemy(0, -200, math.random(0,800), 50) }
  
end


function love.update(dt)
  
  player:keyDown(dt)
  
  for i =1, #enemy_array do
    enemy_array[i]:update(dt)
  end
  
end


function love.draw()
  player.draw()
  enemy_array[1]:draw()
  enemy_array[2]:draw()
  love.graphics.print(tostring(enemy_array[1].width), 500, 500)
end
