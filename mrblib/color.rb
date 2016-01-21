class MiniGame::Color
  def self.gray(gray_scale, alpha=255)
    return Color.rgb(gray_scale, gray_scale, gray_scale, alpha)
  end
end
