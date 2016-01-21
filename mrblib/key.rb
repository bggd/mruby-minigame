module MiniGame::Key

  @@key_pressed = Array.new(300) do |i| false end
  @@key_released = Array.new(300) do |i| false end
  @@key_down = Array.new(300) do |i| false end

  def self.key_pressed=(ary)
    @@key_pressed = ary
  end

  def self.key_released=(ary)
    @@key_released = ary
  end

  def self.key_pressed
    return @@key_pressed
  end

  def self.key_released
    return @@key_released
  end
  
  def self.key_down
    return @@key_down
  end

  def self.pressed(key)
    return @@key_pressed[key]
  end

  def self.released(key)
    return @@key_released[key]
  end

  def self.down(key)
    return @@key_down[key]
  end
end
