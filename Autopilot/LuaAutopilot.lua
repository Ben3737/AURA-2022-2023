-- warn the user if wind speed exceeds a threshold, failsafe if a second threshold is exceeded

-- note that this script is only intended to be run on ArduPlane
local count = 0

function update()
	gcs:send_test(0, count)
	if count > 100 then
		gcs:send_text(0, "Time to go home.")
		vehicle:set_mode(11) -- FIXME: should be an enum.  11 is RTL.
		return
	end
	count = count + 1
   	return update, 1000
end

return update, 1000
