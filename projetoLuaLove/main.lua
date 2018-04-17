
function new_player(x, y, r)

  
  return {
    
    posx = x,
    posy = y, 
    radius = r,    
    score = 0,
    
    dtMult_player = 650,
    
    draw = function (self)      
            love.graphics.circle('fill', self.posx, self.posy, self.radius)
    end,
   
    
    keyDown = function (self, dt)
      
      if love.keyboard.isDown("down") then
        self.posy = self.posy + self.dtMult_player*dt
        if self.posy >= 600 - self.radius then
          self.posy = 600 - self.radius
        end
      end
      if love.keyboard.isDown("up") then
        self.posy = self.posy - self.dtMult_player*dt
        if self.posy - self.radius <= 0 then
          self.posy = 0 + self.radius
        end
      end
      if love.keyboard.isDown("right") then
        self.posx = self.posx + self.dtMult_player*dt
        if self.posx >= 800 - self.radius then
          self.posx = 800 - self.radius
        end
      end
      if love.keyboard.isDown("left") then
        self.posx = self.posx - self.dtMult_player*dt
        if self.posx - self.radius <= 0 then
          self.posx = 0 + self.radius
        end
      end
      
    end 
    
  
  }
end



function new_enemy(x, y, w, h)
   
  return {
    
  spaceBetweenBlocks = 100,
  dtMult_enemy = 250,
  
  posx = x, 
  posy = y,
  width = w,
  height = h,
  score_flag = true,
    
   draw = function (self)
          love.graphics.rectangle('fill', self.posx, self.posy, self.width, self.height)
          love.graphics.rectangle('fill', self.width + self.spaceBetweenBlocks, self.posy, 750 - self.width, self.height)
    end,
    
   randomWidth = function (self)
      self.width = math.random(0,350)
   end,
    
   update = function (self, dt)
      
      self.posy = self.posy + self.dtMult_enemy*dt
      
      if self.posy >= 600 then
        self:randomWidth()
        self.posy = 0 - self.height
        self.score_flag = true
      end
      
    end,  
    
    checkScoreOrCollision = function (self, _player)
    
      --score
      if self.posy - self.height >=  _player.posy - _player.radius then
        if self.score_flag == true then
          _player.score = _player.score + 1
          self.score_flag = false
        end
      --collision
      elseif (_player.posy - _player.radius <= self.posy + self.height) and (_player.posx - _player.radius <= self.posx + self.width or _player.posx + _player.radius>= self.posx + self.width + self.spaceBetweenBlocks) then
        screen = 1
        
      end 
      
    
    end
    
  }

end

function love.keypressed(key)
  
  if screen == 1 then
    if key == 'p' then
      love.load()
    end
  end  
end

function love.load()
   
  screen = 0 
  player = new_player(405, 500, 10)
  enemy_array = {new_enemy(0, 0, math.random(0,700), 30),
                 new_enemy(0, -200, math.random(0,700), 30),
                 new_enemy(0, -400, math.random(0,700), 30)}
  
end


function love.update(dt)
  if screen == 0 then
    player:keyDown(dt)
    
    for i =1, #enemy_array do
      enemy_array[i]:update(dt)
      enemy_array[i]:checkScoreOrCollision(player)
    end
  end
end


function love.draw()
  
  if screen == 0 then  
    player:draw()
    enemy_array[1]:draw()
    enemy_array[2]:draw()
    enemy_array[3]:draw()
    love.graphics.print("Score: ", 10, 10)
    love.graphics.print(tostring(player.score), 60, 10)
  elseif screen == 1 then
    love.graphics.print("Score: ", 350, 300)
    love.graphics.print(tostring(player.score), 400, 300)
    love.graphics.print("Press P to play again", 325, 550)
  
  end
  
  

end
