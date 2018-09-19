#ifndef TY_BOOLEAN_BUTTON_H
#define TY_BOOLEAN_BUTTON_H

namespace ty {
	class boolean_button {
		bool _activated = false;
		bool _justReleased = false;
		bool _justPressed = false;
	public:
		operator bool() const noexcept {
			return _activated;
		}
		void track(bool pushed) noexcept
		{
			_justReleased = false;
			_justPressed = false;
			if (!pushed && _activated)
			{
				_justReleased = true;
			}
			else if (pushed && !_activated)
			{
				_justPressed = true;
			}
			_activated = pushed;
		}
		bool just_released() const noexcept
		{
			return _justReleased;
		}

		bool just_pressed() const noexcept
		{
			return _justPressed;
		}
	};
}

#endif