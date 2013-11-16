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

  @@button_down = {left:false, middle:false, right:false}

  def self.button_down
    return @@button_down
  end

  def self.down(button=:left)
    return @@button_down[button]
  end

end
