module Minigame::Gameloop

  @@quit = false

  def self.quit
    @@quit = true
  end

  @@mouse_pressed_blk = nil
  @@mouse_released_blk = nil

  @@key_pressed_blk = nil
  @@key_released_blk = nil

  @@update_blk = nil
  @@draw_blk = nil

  def self.mouse_pressed(&blk)
    @@mouse_pressed_blk = blk
  end

  def self.mouse_released(&blk)
    @@mouse_released_blk = blk
  end

  def self.key_pressed(&blk)
    @@key_pressed_blk = blk
  end

  def self.key_released(&blk)
    @@key_released_blk = blk
  end

  def self.update(&blk)
    @@update_blk = blk
  end

  def self.draw(&blk)
    @@draw_blk = blk
  end

  def self.run(fps=60)
    running = true

    frame_rate = 1.0/fps

    timer = Minigame::Event::Timer.new(frame_rate)

    Minigame::Event.flush

    timer.start

    update_prev = Minigame.get_time

    while running && !@@quit

      Minigame::Key.key_pressed = Array.new(300) do |i| false end
      Minigame::Key.key_released = Array.new(300) do |i| false end

      Minigame::Mouse.button_pressed = {left:false, middle:false, right:false}
      Minigame::Mouse.button_released = {left:false, middle:false, right:false}

      Minigame::Event.wait do |ev|
        case ev.type
        when Minigame::Event::TIMER
          break
        when Minigame::Event::KEY_DOWN
          Minigame::Key.key_pressed[ev.keyboard.keycode] = true
          Minigame::Key.key_down[ev.keyboard.keycode] = true
          @@key_pressed_blk.call(ev.keyboard.keycode) if @@key_pressed_blk
        when Minigame::Event::KEY_UP
          Minigame::Key.key_released[ev.keyboard.keycode] = true
          Minigame::Key.key_down[ev.keyboard.keycode] = false
          @@key_released_blk.call(ev.keyboard.keycode) if @@key_released_blk
        when Minigame::Event::MOUSE_AXES
          Minigame::Mouse.x = ev.mouse.x
          Minigame::Mouse.y = ev.mouse.y
          #Minigame::Mouse.z = ev.mouse.z
          #Minigame::Mouse.w = ev.mouse.w
          #Minigame::Mouse.dx = ev.mouse.dx
          #Minigame::Mouse.dy = ev.mouse.dy
          #Minigame::Mouse.dz = ev.mouse.dz
          #Minigame::Mouse.dw = ev.mouse.dw
        when Minigame::Event::MOUSE_BUTTON_DOWN, Minigame::Event::MOUSE_BUTTON_UP
          Minigame::Mouse.x = ev.mouse.x
          Minigame::Mouse.y = ev.mouse.y
          #Minigame::Mouse.z = ev.mouse.z
          #Minigame::Mouse.w = ev.mouse.w
          button = case ev.mouse.button
          when 1
            :left
          when 2
            :right
          when 3
            :middle
          end

          b = if ev.type == Minigame::Event::MOUSE_BUTTON_DOWN
                true
              else
                false
              end

          Minigame::Mouse.button_down[button] = b

          if b
            Minigame::Mouse.button_pressed[button] = true
            @@mouse_pressed_blk.call(ev.mouse.x, ev.mouse.y, button) if @@mouse_pressed_blk
          else
            Minigame::Mouse.button_released[button] = true
            @@mouse_released_blk.call(ev.mouse.x, ev.mouse.y, button) if @@mouse_released_blk
          end
        when Minigame::Event::DISPLAY_CLOSE
          running = false
        end
      end

      update_dt = Minigame.get_time - update_prev
      @@update_blk.call(update_dt) if @@update_blk
      update_prev = Minigame.get_time
      #Minigame::Display.clear
      @@draw_blk.call if @@draw_blk
      Minigame::Display.flip
    end
  end
end
