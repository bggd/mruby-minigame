module Minigame::Mouse

  @@x = 0
  @@y = 0

  def self.x
    return @@x
  end

  def self.x=(x)
    @@x = x
  end

  def self.y
    return @@y
  end

  def self.y=(y)
    @@y = y
  end

  @@button_pressed = {left:false, middle:false, right:false}
  @@button_released = {left:false, middle:false, right:false}
  @@button_down = {left:false, middle:false, right:false}

  def self.button_pressed=(hash)
    @@button_pressed = hash
  end

  def self.button_released=(hash)
    @@button_released = hash
  end

  def self.button_pressed
    return @@button_pressed
  end

  def self.button_released
    return @@button_released
  end

  def self.button_down
    return @@button_down
  end

  def self.pressed(button=:left)
    return @@button_pressed[button]
  end

  def self.released(button=:left)
    return @@button_released[button]
  end

  def self.down(button=:left)
    return @@button_down[button]
  end

end
