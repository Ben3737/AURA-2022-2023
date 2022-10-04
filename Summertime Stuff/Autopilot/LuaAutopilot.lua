-- note that this script is only intended to be run on ArduPlane
local count = 0

function update()
	gcs:send_text(0, count)
	gcs:send_text(0, rc:get_pwm(1))
	if count > 10000 then
		gcs:send_text(0, "Time to go home.")
		vehicle:set_mode(11) -- FIXME: should be an enum.  11 is RTL.
		return update, 1000
	end
	count = count + 1
   	return update, 1000
end

return update, 1000
