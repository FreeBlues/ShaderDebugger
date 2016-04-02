-- Shader debug
displayMode(OVERLAY)
function setup()
    m = mesh()

    m:addRect(WIDTH/2,HEIGHT/2,WIDTH,HEIGHT)    

    m.shader = shader(myShader.vsBase,myShader.fsBase)
    
    -- m.texture = "Documents:univer"
    m:setColors(color(220,200,200,255))
    
    parameter.watch("m.shader.modelViewProjection")
    parameter.watch("m.shader.uResolution")
    parameter.watch("m.vertices[1]")
end

function draw()
    background(0)
    m:draw()
end

function touched(touch)
    
end
