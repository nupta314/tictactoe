#include "frontend.hpp"

using namespace std;

//AI is kinda incomplete
//makes suboptimal decisions
//change the expression of a in Status::refresh() function
//and the nextmove() function
//for better results

//GUI is almost complete
//there are some minor bugs
//if user does not make mistake neither will the gui
//and next time when making something like this think it through from the begining
//and don't do it along the way
//it works(barely) but if user makes a mistake like clicking same button twice it will no work as it should

int main(int argc, char *argv[]) {
	Glib::RefPtr<Gtk::Application> app = Gtk::Application::create(argc, argv,"org.gtkmm.examples.base");
	TicTacToe window;
	return app->run(window);
}
