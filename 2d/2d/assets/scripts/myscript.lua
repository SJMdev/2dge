-- This is a lua global variable
some_variable = 7 * 6
user_name = "Sjors"
print("hello "..user_name)
if some_variable == 42 then
	print("the value of the variable is " ..some_variable)
end

-- this is a lua table that contain values for the initialization of our game engine. 
config = {
	title = "My Game Engine",
	fullscreen = false,
	resolution = {
		width = 800,
		height = 600
	},
	
}


-- this is a lua function that calculates the factorial of a given number
function factorial(n)
	if n == 1 then return 1 end
	return n * factorial(n - 1)
end