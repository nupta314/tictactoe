#ifndef FRONTEND_HPP_INCLUDED
#define FRONTEND_HPP_INCLUDED

#include "backend.hpp"

#include <gtkmm.h>

bool playerfirst=false;
State playermove=State::blank,computermove=State::blank;
std::array<State,9> abs_grid= {State::blank,State::blank,State::blank,State::blank,State::blank,State::blank,State::blank,State::blank,State::blank};
int fin=0;

//after adding a widget show() it otherwise it will not be there
//basic display ready?

class Button : public Gtk::Button {
	int pos{};
  public:
	Button() {
		set_image(*(new Gtk::Image("resources/blank.png")));
		signal_clicked().connect(sigc::mem_fun(*this,&Button::on_click));
	}
	Button(int posi):pos(posi) {
		set_image(*(new Gtk::Image("resources/blank.png")));
		signal_clicked().connect(sigc::mem_fun(*this,&Button::on_click));
	}
	void setp(int p) {
		pos=p;
	}
	void sets(State state=State::blank) {
		if(!(bool)abs_grid[pos]) {
			abs_grid[pos]=state;
		}
		std::string img = "resources/";
		img += (state==State::circle)?"O.png":(state==State::cross)?"X.png":"blank.png";
		set_image(*(new Gtk::Image(img)));
	}
	void on_click() {
		sets(playermove);
		fin++;
	}
};

class TicTacToe : public Gtk::Window {
	Gtk::Grid gridbase;
	std::array<Button,9> disp_grid;
	Gtk::Button refresh;
	void on_button_refresh_clicked();
  public:
	void play();
	TicTacToe() {
		set_icon_from_file("resources/icon.png");
		set_title("TicTacToe");
		set_border_width(0);
		set_resizable(false);

		refresh.set_image_from_icon_name("view-refresh");
		refresh.show();
		gridbase.attach(refresh,0,0,3,1);
		refresh.signal_clicked().connect(sigc::mem_fun(*this,&TicTacToe::on_button_refresh_clicked));

		for(int y:std::views::iota(0,3)) {
			for(int x:std::views::iota(0,3)) {
				disp_grid[3*y+x].signal_clicked().connect(sigc::mem_fun(*this,&TicTacToe::play));
				disp_grid[3*y+x].setp(3*y+x);
				disp_grid[3*y+x].show();
				gridbase.attach(disp_grid[3*y+x],x,y+1);
			}
		}
		gridbase.show();
		add(gridbase);

		on_button_refresh_clicked();
	}
};

void TicTacToe::on_button_refresh_clicked() {
	Gtk::MessageDialog dialog(*this,"Will you have the first turn?",false,
							  Gtk::MessageType::MESSAGE_QUESTION,
							  Gtk::ButtonsType::BUTTONS_NONE);
	dialog.add_button("Yes",1);
	dialog.add_button("No",0);
	dialog.set_default_response(0);
	int answer = dialog.run();
	if(answer>=0) {
		playerfirst=answer;
		std::ranges::for_each(disp_grid,[answer](Button& a) {return a.sets();});
	}

	playermove=playerfirst?State::cross:State::circle;
	computermove=playerfirst?State::circle:State::cross;

	if(!playerfirst) {
		play();
	}
}

void TicTacToe::play() {
	State winner = check_winner(abs_grid);
	if(!(bool)winner){
		int next_move = nextmove(abs_grid);
		if(fin<9){
			abs_grid[next_move]=computermove;
			disp_grid[next_move].sets(computermove);
			fin++;
		}
		winner = check_winner(abs_grid);
	}
	if((bool)winner||(fin>8)) {
		std::string result = winner==playermove?"You win":winner==computermove?"You lose":"It's a tie";
		Gtk::MessageDialog dialog(*this,result,false,Gtk::MessageType::MESSAGE_INFO,Gtk::ButtonsType::BUTTONS_NONE);
		dialog.run();
		abs_grid = {State::blank,State::blank,State::blank,State::blank,State::blank,State::blank,State::blank,State::blank,State::blank};
		possibilities = initialize_possibilities();
		fin=0;

		on_button_refresh_clicked();
	}
}

/*
int main(int argc, char *argv[]) {
	Glib::RefPtr<Gtk::Application> app = Gtk::Application::create(argc, argv,"org.gtkmm.examples.base");
	TicTacToe window;
	return app->run(window);
}
*/

#endif // FRONTEND_HPP_INCLUDED
