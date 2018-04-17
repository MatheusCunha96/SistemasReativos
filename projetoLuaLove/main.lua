
function new_player(x, y, r)

  
  return {
    
    posx = x,
    posy = y, 
    radius = r,    
    score = 0,
    
    dtMult_player = 400,
    
    draw = function (self)      
            love.graphics.circle('fill', self.posx, self.posy, self.radius)
    end,
   
    
    keyDown = function (self, dt)
      
      if love.keyboard.isDown("down") then
        self.posy = self.posy + self.dtMult_player*dt
        if self.posy >= HEIGHT - self.radius then
          self.posy = HEIGHT - self.radius
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
        if self.posx >= WIDTH - self.radius then
          self.posx = WIDTH - self.radius
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
    
  spaceBetweenBlocks = WIDTH/6,
  dtMult_enemy = 250,
  
  posx = x, 
  posy = y,
  width = w,
  height = h,
  score_flag = true,
    
   draw = function (self)
          love.graphics.rectangle('fill', self.posx, self.posy, self.width, self.height)
          love.graphics.rectangle('fill', self.width + self.spaceBetweenBlocks, self.posy, WIDTH - self.width, self.height)
    end,
    
   randomWidth = function (self)
      self.width = math.random(0,WIDTH/2)
   end,
    
   update = function (self, dt)
      
      self.posy = self.posy + self.dtMult_enemy*dt
      
      if self.posy >= HEIGHT then
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
  
  WIDTH = 400
  HEIGHT = 1000
  
  love.window.setMode(WIDTH,HEIGHT)
  
  screen = 0 
  player = new_player(WIDTH/2, HEIGHT-100, 10)
  
  enemy_array = {}
  
  for i=1, HEIGHT/250 do
    enemy_array[i] = new_enemy(0, -250*i, math.random(0,WIDTH), 30)
  
  end
  
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
    for i =1, #enemy_array do
      enemy_array[i]:draw()
    end
    love.graphics.print("Score: ", 10, 10)
    love.graphics.print(tostring(player.score), 60, 10)
  elseif screen == 1 then
    love.graphics.print("Score: ", WIDTH/2 -40 , HEIGHT/2 - 100)
    love.graphics.print(tostring(player.score), WIDTH/2 + 10, HEIGHT/2 - 100)
    love.graphics.print("Press P to play again", WIDTH/2 - 75 , HEIGHT/2 + 100)
  
  end
  
  

end
