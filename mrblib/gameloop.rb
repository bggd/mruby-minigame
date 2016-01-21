module MiniGame::GameLoop

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

    redraw = true

    frame_rate = 1.0/fps

    timer = MiniGame::Event::Timer.new(frame_rate)

    MiniGame::Event.flush

    timer.start

    update_prev = MiniGame.get_time

    while running && !@@quit

      MiniGame::Key.key_pressed = Array.new(300) do |i| false end
      MiniGame::Key.key_released = Array.new(300) do |i| false end

      MiniGame::Mouse.button_pressed = {left:false, middle:false, right:false}
      MiniGame::Mouse.button_released = {left:false, middle:false, right:false}

      MiniGame::Event.wait do |ev|
        case ev.type
        when MiniGame::Event::TIMER
          redraw = true
        when MiniGame::Event::KEY_DOWN
          MiniGame::Key.key_pressed[ev.keyboard.keycode] = true
          MiniGame::Key.key_down[ev.keyboard.keycode] = true
          @@key_pressed_blk.call(ev.keyboard.keycode) if @@key_pressed_blk
        when MiniGame::Event::KEY_UP
          MiniGame::Key.key_released[ev.keyboard.keycode] = true
          MiniGame::Key.key_down[ev.keyboard.keycode] = false
          @@key_released_blk.call(ev.keyboard.keycode) if @@key_released_blk
        when MiniGame::Event::MOUSE_AXES
          MiniGame::Mouse.x = ev.mouse.x
          MiniGame::Mouse.y = ev.mouse.y
          #MiniGame::Mouse.z = ev.mouse.z
          #MiniGame::Mouse.w = ev.mouse.w
          #MiniGame::Mouse.dx = ev.mouse.dx
          #MiniGame::Mouse.dy = ev.mouse.dy
          #MiniGame::Mouse.dz = ev.mouse.dz
          #MiniGame::Mouse.dw = ev.mouse.dw
        when MiniGame::Event::MOUSE_BUTTON_DOWN, MiniGame::Event::MOUSE_BUTTON_UP
          MiniGame::Mouse.x = ev.mouse.x
          MiniGame::Mouse.y = ev.mouse.y
          #MiniGame::Mouse.z = ev.mouse.z
          #MiniGame::Mouse.w = ev.mouse.w
          button = case ev.mouse.button
          when 1
            :left
          when 2
            :right
          when 3
            :middle
          end

          b = if ev.type == MiniGame::Event::MOUSE_BUTTON_DOWN
                true
              else
                false
              end

          MiniGame::Mouse.button_down[button] = b

          if b
            MiniGame::Mouse.button_pressed[button] = true
            @@mouse_pressed_blk.call(ev.mouse.x, ev.mouse.y, button) if @@mouse_pressed_blk
          else
            MiniGame::Mouse.button_released[button] = true
            @@mouse_released_blk.call(ev.mouse.x, ev.mouse.y, button) if @@mouse_released_blk
          end
        when MiniGame::Event::DISPLAY_CLOSE
          running = false
        end

        break if redraw && Event.empty?
      end

      update_dt = MiniGame.get_time - update_prev
      @@update_blk.call(update_dt) if @@update_blk
      update_prev = MiniGame.get_time
      #MiniGame::Display.clear
      @@draw_blk.call if @@draw_blk
      MiniGame::Display.flip
      redraw = false
    end
  end
end
