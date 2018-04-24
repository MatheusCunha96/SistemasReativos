led1 = 3
led2 = 6
sw1 = 1
sw2 = 2
nowUp = 0
nowDown = 1000000

ledBlinkTimer = 1000

local timer = tmr.create()

gpio.mode(led1, gpio.OUTPUT)
gpio.mode(led2, gpio.OUTPUT)

gpio.write(led1, gpio.LOW);
gpio.write(led2, gpio.LOW);

gpio.mode(sw1,gpio.INT,gpio.PULLUP)
gpio.mode(sw2,gpio.INT,gpio.PULLUP)

function newpincb (led)
  local ledstate = false
  
  return
  function ()
    ledstate =  not ledstate
    if ledstate then 
      gpio.write(led, gpio.HIGH);
    else
      gpio.write(led, gpio.LOW);
    end
  end
end

function changeTime(state)

    return function ()     
    if state == "up" then
        ledBlinkTimer = ledBlinkTimer/2
        nowUp = tmr.now()
        print(tostring(nowUp)..' Up')
    elseif state == "down" then
        ledBlinkTimer = ledBlinkTimer*2
        nowDown = tmr.now()
        print(tostring(nowDown)..' Down')
    end
        
    timer:alarm(ledBlinkTimer, tmr.ALARM_AUTO, newpincb(led2))
    print(ledBlinkTimer) 
TEMPO = math.abs(nowUp - nowDown)
print("INTERVALO", TEMPO)
    if TEMPO <= 300000 then
        print('STOP')
        gpio.write(led2, gpio.LOW)
        timer:stop()
    end
    
    end

end

gpio.trig(sw1, "down", changeTime("up"))
gpio.trig(sw2, "down", changeTime("down"))

timer:alarm(ledBlinkTimer, tmr.ALARM_AUTO, newpincb(led2))
