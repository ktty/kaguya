glfw=require("luaglfw")
nvg=require("luananovg")

if glfw.Init() == false then
   return -1;
end

    -- Create a windowed mode window and its OpenGL context
    window = glfw.CreateWindow(640, 480, "Hello World", 0, 0);
    if (window == nil) then
        glfw.Terminate();
        return -1;
    end
    drawpath={}
    glfw.SetCursorPosCallback(window,function(x,y) table.insert(drawpath,{x,y}) end)
    glfw.SetWindowPosCallback(window,function(x,y) end)
    glfw.SetWindowCloseCallback(window,function() print("window closed!") end)

    glfw.MakeContextCurrent(window);
    context = nvg.Create(0);
    while (glfw.WindowShouldClose(window) == 0) do
        local width,height = glfw.GetFramebufferSize(window);
        nvg.BeginFrame(context,width, height,1);
        local red = nvg.RGB(255,0,0);
        nvg.StrokeColor(context,red);
        nvg.BeginPath(context);        
        for i,var in ipairs(drawpath) do   
          if i==1 then
            nvg.MoveTo(context,var[1],var[2]);
          end
        nvg.LineTo(context,var[1],var[2]);
        end
        nvg.StrokeWidth(context,5);
        nvg.Stroke(context);
        nvg.EndFrame(context);
        
        -- Swap front and back buffers 
        glfw.SwapBuffers(window);
        

        -- Poll for and process events 
        glfw.PollEvents();
    end

    glfw.Terminate();
    return 0;