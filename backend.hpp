#ifndef BACKEND_HPP_INCLUDED
#define BACKEND_HPP_INCLUDED

#include <iostream>
#include <algorithm>
#include <functional>
#include <fstream>
#include <ranges>
#include <bitset>
#include <set>
#include <map>

enum class State {
	blank =0,
	circle =2,
	cross =1
};

void switchS(State& s){
	if(!(bool)s)return;
	s=(s==State::circle)?State::cross:State::circle;
}

std::array<State,9> i2sarr(std::array<int,9> input) {
	std::array<State,9> output= {State::blank,State::blank,State::blank,State::blank,State::blank,State::blank,State::blank,State::blank,State::blank};
	for(int i=0; i<9; ++i) {
		output[i]=(State)input[i];
	}
	return output;
}

enum class Winner {
	unfinished = 0,
	one_wins = 1,//first player wins
	two_wins = 2,//second player wins
	tie = 3
};

char state2char(const State& state) {
	if((bool)state)
		return state==State::circle?'O':'X';
	else return '-';
}

void printstate(const std::array<State,9>& a) {
	for(int i:std::views::iota(0,3)) {
		for(int j:std::views::iota(0,3)) {
			std::cout<<state2char(a[i*3+j])<<'\t';
		}
		std::cout<<'\n';
	}
}

struct board_state {
	std::array<State,9> turns;
	int turn;
	Winner winner;
};

struct board_state_comparer {
	bool operator()(const board_state& lhs,const board_state& rhs)const {
		return lhs.turns<rhs.turns;
	}
};

typedef std::set<board_state,board_state_comparer> posses;

State check_winner(std::array<State,9> input) {
	if((bool)input[4]) {
		if(((input[0]==input[4])&&(input[4]==input[8]))||
				((input[3]==input[4])&&(input[4]==input[5]))||
				((input[6]==input[4])&&(input[4]==input[2]))||
				((input[7]==input[4])&&(input[4]==input[1]))) {
			return input[4];
		}
	}
	if((bool)input[0]) {
		if(((input[0]==input[3])&&(input[3]==input[6]))||
				((input[0]==input[1])&&(input[1]==input[2]))) {
			return input[0];
		}
	}
	if((bool)input[8]) {
		if(((input[2]==input[5])&&(input[5]==input[8]))||
				((input[6]==input[7])&&(input[7]==input[8]))) {
			return input[8];
		}
	}
	return State::blank;
}

template<typename T,size_t N>
int index(std::array<T,N> a,T b) {
	return std::distance(std::begin(a),std::ranges::find(a,b));
}

board_state check_winning(std::array<int,9> input) {
	int turn=0;
	Winner winner = Winner::unfinished;
	std::array<State,9> base= {State::blank,State::blank,State::blank,State::blank,State::blank,State::blank,State::blank,State::blank,State::blank};
	for(; turn<9; turn++) {
		base[index(input,turn)] = turn%2?State::circle:State::cross;
		if((bool)check_winner(base))
			break;
	}
	if(turn==9)
		winner = Winner::tie;
	else
		winner = (Winner)check_winner(base);
	return board_state{base,turn,winner};
}

posses initialize_possibilities() {
	posses board_states;
	std::array<int,9> board{0,1,2,3,4,5,6,7,8};
	do {
		board_states.insert(check_winning(board));
	}
	while(std::ranges::next_permutation(board).found);
	//cout<<board_states.size()<<"\n";
	//size should be 765 but is 974 -> 958
	//some error causing more than actual formations
	//correct it
	//most probably the error is in the check_winning_turn or check_winner
	return board_states;
}

posses possibilities = initialize_possibilities();

bool matches(std::array<State,9> currentmoves,std::array<State,9> possmoves) {
	for(int i=0; i<9; ++i) {
		if((bool)currentmoves[i]) {
			if(currentmoves[i]!=possmoves[i])return false;
		}
	}
	return true;
}

posses possibility_minimizer(std::array<State,9> input) {
	posses local_poss;
	for(board_state i:possibilities) {
		if(matches(input,i.turns)) {
			local_poss.insert(i);
		}
	}
	return local_poss;
}

struct status {
	float w,l,d,t,a;
	status() {
		w=0;
		l=0;
		d=0;
		t=1;
		a=0;
	}
	void refresh() {
		t=w+l+d;
		a=1/(l*t);
		//start logging expressions of a and the results
	}
};

std::map<int,status> stats(std::array<State,9> input) {
	posses local_poss = possibility_minimizer(input);
	std::map<int,status> answer{};
	std::set<int> blanks{};
	for(int i=0; i<9; ++i) {
		if(input[i]==State::blank) {
			blanks.insert(i);
		}
	}
	State next_turn = (State)(2 - (blanks.size()%2));
	for(const board_state& l_poss:local_poss) {
		for(const int& blank:blanks) {
			if(l_poss.turns[blank]==next_turn) {
				if(l_poss.winner==Winner::tie)
					answer[blank].d+=1;
				else if(l_poss.winner==(Winner)next_turn) {
					answer[blank].w+=1;
				}
				else {
					answer[blank].l+=1;
				}
			}
		}
	}
	for(auto& [k,v]:answer) {
		v.refresh();
	}
	return answer;
}

int nextmove(std::array<State,9> input){
	std::map<int,status> temp = stats(input);
	return std::ranges::max_element(temp,[](auto& lhs,auto& rhs) {return lhs.second.a<rhs.second.a;})->first;
}

/*
int main() {
	array<State,9> t=i2sarr(array<int,9>{});
	int c=0;
	int next_move=0;
	cout<<"Will you play first? ";
	string response;
	cin>>response;
	State playermove,computermove;
	playermove=response.compare("yes")?State::circle:State::cross;
	computermove=response.compare("yes")?State::cross:State::circle;
	printstate(t);

	if(response.compare("yes"))goto gtx;
	while(true){
		do{
			cout<<"Enter your choice (0-8) ";
			int temp;
			cin>>temp;
			if(temp<0||temp>8)continue;
			else next_move=temp;
		}while((bool)t[next_move]);
		t[next_move]=playermove;
		printstate(t);
		if(++c==9)break;
		if((bool)check_winner(t))break;

//there are some places where it makes suboptimal decisions and loses
//change value of a in refresh function of status for avoiding losses

		gtx:{
		map<int,status> a=stats(t);
		next_move = ranges::max_element(a,[](auto& lhs,auto& rhs){return lhs.second.a<rhs.second.a;})->first;
		t[next_move]=computermove;
		cout<<endl;
		printstate(t);
		if(++c==9)break;
		if((bool)check_winner(t))break;
		}
	}
	cout<<endl;
	check_winner(t)==playermove?cout<<"You win\n":
	check_winner(t)==computermove?cout<<"You lose\n":
	cout<<"It's a tie\n";
	return 0;
}
*/

#endif // BACKEND_HPP_INCLUDED
