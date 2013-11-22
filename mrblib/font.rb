class Minigame::Font


  def to_image(text, color=Minigame::Color.rgb)

    img = Minigame::Image.new(text_width(text), h(), Minigame::Color.rgb(0, 0, 0, 0))

    Minigame::Image.target(img) do
      draw 0, 0, text, color:color
    end

    return img
  end
end
