#include "StateMachine.h"

#include <iostream>
#include <chrono>
#include <thread>

void func(int_fast8_t id, const std::string& method) {
	std::cout << "[State " << (int)id << "] " << method << std::endl;
}

int main() {
	StateMachine<5, 2> fsm{ StateBuilder<2>(0,
		[]() { func(0, "init"); },
		[]() { func(0, "update"); },
		[]() { func(0, "deinit"); } )
		.addTransition(1, []() { func(0, "Transition to 1");
			return true;
		})
		.addTransition(2, []() { func(0, "Transition to 2");
			return false;
		})
		.build() };
	
	fsm.addState( StateBuilder<2>(1,
		[]() { func(1, "init"); },
		[]() { func(1, "update"); },
		[]() { func(1, "deinit"); } )
		.addTransition(2, []() { func(1, "Transition to 2");
			return true;
		})
		.build() );
	fsm.addState( StateBuilder<2>(2,
		[]() { func(2, "init"); },
		[]() { func(2, "update"); },
		[]() { func(2, "deinit"); } )
		.addTransition(1, []() { func(2, "Transition to 1");
			return false;
		})
		.addTransition(3, []() { func(2, "Transition to 3");
			return true;
		})
		.build() );
	fsm.addState( StateBuilder<2>(3,
		[]() { func(3, "init"); },
		[]() { func(3, "update"); },
		[]() { func(3, "deinit"); } )
		.addTransition(4, []() { func(3, "Transition to 4");
			return true;
		})
		.build() );
	fsm.addState( StateBuilder<2>(4,
		[]() { func(4, "init"); },
		[]() { func(4, "update"); },
		[]() { func(4, "deinit"); } )
		.addTransition(3, []() { func(4, "Transition to 3");
			return false;
		})
		.addTransition(2, []() { func(4, "Transition to 2");
			return false;
		})
		.build() );

	while(true) {
		fsm.run();

		std::this_thread::sleep_for(std::chrono::seconds(1));
	}

	return 0;
}
