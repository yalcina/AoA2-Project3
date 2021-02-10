#include <stdio.h>
#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <ctime>
#include <fstream>
#include <climits>

using namespace std;


int TEAM_NUM = 0;
int* CURRENT = NULL;
int** MATCH_MATRIX = NULL;


class  Node {  //  Generic Node Class for Source, Match, Team and Destination Nodes
  public:
	char  node_type;	//  's' for source node,    'm' for match nodes,    't' for team nodes     and    'd' for destination node
	bool  visited;		//	true if node is visited(i.e pushed to the stack), false otherwise
	
	
  public:		//  part only for source node
	Node **Matches;
	int	 *cap_to_matches;
	
	
  public:		//  part only for match nodes
	Node *Src;
	int	 cap_to_source;
	
	Node *First_Team;
	int	 cap_to_first;
	
	Node *Second_Team;
	int	 cap_to_second;
	
	
  public:		//  part only for team nodes
	Node **Matchess;
	int	 *cap_to_matchess;
	
	Node *Dest;
	int	 cap_to_destination;
	
	
  public:		//  part only for destination node
	Node **Teams;
	int	 *cap_to_teams;

  
  public:
	Node();									//  Default Constructor
	~Node();								//	Destructor
	void  pseudo_constructor(char, int);	//  Pseudo Constructor for incapacity in built-in constructors such as inability to initialize array of objects of arbitrary size
	bool  has_child_named_and_capacity_to(Node*);
	void  print_data(){
		cout << endl << "  Node Type :  "  << node_type << "\tVisited :  "  << visited << endl;
	}
};



Node::Node(){ // Default Constructor for deactivating all data members
	node_type = '\0';
	visited = false;
	
	Matches = NULL;
	cap_to_matches = NULL;
	
	Src = NULL;
	cap_to_source = -1;
	
	First_Team = NULL;
	cap_to_first = -1;
	
	Second_Team = NULL;
	cap_to_second = -1;
	
	Matchess = NULL;
	cap_to_matchess = NULL;
	
	Dest = NULL;
	cap_to_destination = -1;
	
	Teams = NULL;
	cap_to_teams = NULL;
}



void  Node::pseudo_constructor(char typ, int num){ //  Pseudo Constructor for activating some of data members according to the chosen node type

	node_type = typ;
	
	if(node_type == 's'){
		Matches = new Node* [(num*(num-1))/2];					//   Node **
		cap_to_matches = new int [(num*(num-1))/2];				//	 int *
	}
	
	else if(node_type == 'm'){
	}
	
	else if(node_type == 't'){
		Matchess = new Node* [num-1];							// Node **
		cap_to_matchess = new int [num-1];						// 	int	*
	}
	
	else if(node_type == 'd'){
		Teams = new Node* [num];								// Node **
		cap_to_teams = new int [num];							// 	int	*
	}
	
	else {
		cout << "\nERROR in type!!" << endl << endl;
	}
}


Node::~Node(){
	delete [] Matches;
	delete [] cap_to_matches;
	delete [] Matchess;
	delete [] cap_to_matchess;
	delete [] Teams;
	delete [] cap_to_teams;
}


bool  Node::has_child_named_and_capacity_to(Node* in){

	if(node_type == 's'){
		for(int i = 0; i < (TEAM_NUM*(TEAM_NUM-1))/2; i++){
			if(Matches[i] == in){
				if(cap_to_matches[i] > 0){
					return true;
				}
			}
		}
	}
	
	else if(node_type == 'm'){

		if(First_Team == in){
			if(cap_to_first > 0){
				return true;
			}
		}
		if(Second_Team == in){
			if(cap_to_second > 0){
				return true;
			}
		}

	}
	
	else if(node_type == 't'){
		if(Dest == in){
			if(cap_to_destination > 0){
				return true;
			}
		}

		for(int j = 0; j < TEAM_NUM-1; j++){
			if(Matchess[j] == in){
				if(cap_to_matchess[j] > 0){
					return true;
				}
			}
		}
	}

	else if(node_type == 'd'){
	}

	return false;
}

//-------------------------------------------------------------

class  League {
  public:
	int  N;					//   i.e  # of teams
	int for_team_i;			//	 whole max-flow network to be constructed according to the i'th team
	int max_score_for_i;	//	 = current + max_winning
	int maximum;			//	 sum of all source flows

	int* src_flows;
	int* dest_flows;

	Node *s;				//	 Dummy Source Node
	Node *matches;			//	 Match Nodes
	Node *teams;			//	 Team Nodes
	Node *d;				//	 Dummy Destination Node

	Node **Stack;			//	 for DFS search
	int top_stack;			//	 top element in Stack

	Node **Path_List;		//	 for keeping track of found path from source to destination
	int top_path;			//	 top element in Path List
	
	
  public:
	League(int, int);			//  Default Constructor
	~League();					//	Destructor
	int   IS_IT_POSSIBLE_FOR_TEAM_i_TO_WIN();
	int   run_Ford_Fulkerson();
	void  find_teams(int, int&, int&);
	void  connect_all_nodes_and_give_capacities();
	void  print_network();
	void  print_connections();
	bool  is_there_any_remaining_source_flow();
	void  make_all_nodes_unvisited();
	void  clear_stack_and_path();
	bool  is_stack_empty();		//  to be called prior to popping element from stack. If stack is empty, then no further path to destination cannot be found. As a result, we're done with searching path.
	void  push_to_stack(Node*);
	Node* pop_from_stack();
	void  add_to_path(Node*);
	bool  find_a_path();										
	bool  add_its_appropriate_children_to_stack(Node*);			
	int   find_bottleneck_along_the_path();						
	void  search_for_parent_and_arrange_path(Node*);
	void  send_some_flow(int);									
	void  validate_if_it_is_correct_path();
	void  print_path();
};


void  League::push_to_stack(Node* in){
	Stack[top_stack] = in;
	top_stack++;
}


Node*  League::pop_from_stack(){
	top_stack--;
	Node *ret = Stack[top_stack];

	return ret;
}


void  League::add_to_path(Node* in){
	Path_List[top_path] = in;
	top_path++;
}


bool  League::find_a_path(){
	Node* popped;
	bool affirmative;

	while(true){
		if( is_stack_empty() ){
			return false;
		}

		popped = pop_from_stack();
		add_to_path(popped);

		if(popped == d){
			return true;
		}
		affirmative = add_its_appropriate_children_to_stack(popped);

		if( affirmative == false ){
			if( top_stack > 0 ){
				search_for_parent_and_arrange_path(Stack[top_stack-1]);
			}
		}

	}
}


bool  League::add_its_appropriate_children_to_stack(Node* in){
	bool found = false;

	if(in->node_type == 's'){
		for(int i = 0; i < (N*(N-1))/2; i++){
			if((((in->Matches)[i])->visited == false) && ((in->cap_to_matches)[i] > 0)){
				push_to_stack((in->Matches)[i]);
				((in->Matches)[i])->visited = true;
				found = true;
			}
		}
	}

	else if(in->node_type == 'm'){
		if(((in->First_Team)->visited == false) && (in->cap_to_first > 0)){
				push_to_stack(in->First_Team);
				(in->First_Team)->visited = true;
				found = true;
		}
		if(((in->Second_Team)->visited == false) && (in->cap_to_second > 0)){
				push_to_stack(in->Second_Team);
				(in->Second_Team)->visited = true;
				found = true;
		}
	}

	else if(in->node_type == 't'){
		for(int j = 0; j < N-1; j++){
			if((((in->Matchess)[j])->visited == false) && ((in->cap_to_matchess)[j] > 0)){
				push_to_stack((in->Matchess)[j]);
				((in->Matchess)[j])->visited = true;
				found = true;
			}
		}

		if(((in->Dest)->visited == false) && (in->cap_to_destination > 0)){
				push_to_stack(in->Dest);
				(in->Dest)->visited = true;
				found = true;
		}
	}
	return found;
}


void  League::search_for_parent_and_arrange_path(Node* in){
	int path_cap = ((N*N) + N + 4)/2;
	bool has;
	int ind;

	for(ind = 0; ind < path_cap; ind++){
		has = (Path_List[ind])->has_child_named_and_capacity_to(in);
		if(has){
			break;
		}
	}

	top_path = ind + 1;

	for(int i = top_path; i < path_cap; i++){
		Path_List[i] = NULL;
	}
}




League::League(int team_num = 0, int team_i = 0){ // Default Constructor

	if( team_num < 2  ||  team_num > 10 ){
		cout << endl << "ERROR :   Incorrect number of team!!" << endl;
		return;
	}
	
	N = team_num;
	for_team_i = team_i;

	src_flows = new int [(N*(N-1))/2];
	dest_flows = new int [N];
	
	
	for(int i = 0; i < (N*(N-1))/2; i++){
		int sum = 0;
		int j;
		for(j = 0; j < N-1; j++){
			sum += N-1-j;
			if(i < sum){
				break;
			}
		}
		src_flows[i] = MATCH_MATRIX[j][i-sum+N];
	}
	
	max_score_for_i = CURRENT[for_team_i];
	
	for(int o = 0; o < N; o++){
		max_score_for_i += MATCH_MATRIX[for_team_i][o];
	}

	for(int k = 0; k < N; k++){
		dest_flows[k] = max_score_for_i - CURRENT[k];
	}
	
	
	maximum = 0;
	for(int y = 0; y < (N*(N-1))/2; y++){
		maximum += src_flows[y];
	}

	s = new Node();								//	Node *
	matches = new Node[(N*(N-1))/2];			//	Node *
	teams = new Node[N];						//	Node *
	d = new Node();								//	Node *

	s->pseudo_constructor('s', N);
	d->pseudo_constructor('d', N);
	
	for(int i = 0; i < (N*(N-1))/2; i++){
		matches[i].pseudo_constructor('m', N);
	}
	for(int j = 0; j < N; j++){
		teams[j].pseudo_constructor('t', N);
	}

	Stack = new Node* [ ((N*N) + N + 4)/2 ];				//	Node **
	top_stack = 0;											//	int 
	
	Path_List = new Node* [ ((N*N) + N + 4)/2 ];			//	Node **
	top_path = 0;											//	int 
}


League::~League(){
	delete [] src_flows;
	delete [] dest_flows;
	delete s;
	delete [] matches;
	delete [] teams;
	delete d;
	delete [] Stack;
	delete [] Path_List;
}


int  League::IS_IT_POSSIBLE_FOR_TEAM_i_TO_WIN(){
	int possibility = run_Ford_Fulkerson();
	return possibility;
}


int  League::run_Ford_Fulkerson(){
	bool remaining;
	
	for(int i = 0; i < N; i++){
		if(CURRENT[i] > max_score_for_i){
			return 0;
		}
	}

	while(true){

		remaining = is_there_any_remaining_source_flow();

		if( !(remaining) ){
			return 1;
		}

		make_all_nodes_unvisited();
		clear_stack_and_path();
		push_to_stack(s);
		s->visited = true;

		bool found = find_a_path();

		if(found){
			int flw = find_bottleneck_along_the_path();

			if(flw <= 0){
				cout << endl << "Something wrong in somewhere, find_bottleneck() function couldn't return 0 or negative value!!" << endl;
				getchar();
			}

			send_some_flow(flw);
		}
		else { // if path not found
			return 0;
		}
	}
}


int  League::find_bottleneck_along_the_path(){

	int bottleneck = INT_MAX;
	int index = 0;

	while(true){
		if((Path_List[index])->node_type == 's'){
			for(int i = 0; i < (N*(N-1))/2; i++){
				if( (s->Matches)[i] == Path_List[index+1] ){
					if( (s->cap_to_matches)[i] < bottleneck){
						bottleneck = (s->cap_to_matches)[i];
					}
					break;
				}
			}
		}

		else if((Path_List[index])->node_type == 'm'){
			if( (Path_List[index])->First_Team == Path_List[index+1] ){
				if( (Path_List[index])->cap_to_first < bottleneck){
					bottleneck = (Path_List[index])->cap_to_first;
				}
			}

			else if( (Path_List[index])->Second_Team == Path_List[index+1] ){
				if( (Path_List[index])->cap_to_second < bottleneck){
					bottleneck = (Path_List[index])->cap_to_second;
				}
			}
		}

		else if((Path_List[index])->node_type == 't'){
			if( (Path_List[index])->Dest == Path_List[index+1] ){
				if( (Path_List[index])->cap_to_destination < bottleneck){
					bottleneck = (Path_List[index])->cap_to_destination;
				}
			}

			else {
				for(int i = 0; i < N-1; i++){
					if( ((Path_List[index])->Matchess)[i] == Path_List[index+1] ){
						if( ((Path_List[index])->cap_to_matchess)[i] < bottleneck){
							bottleneck = ((Path_List[index])->cap_to_matchess)[i];
						}
					}
				}
			}
		}

		else if((Path_List[index])->node_type == 'd'){
			break;
		}

		index++;
	}
	return bottleneck;
}



void  League::send_some_flow(int flow){

	int index = 0;

	//	Decrementing flows
	while(true){
		if((Path_List[index])->node_type == 's'){
			for(int i = 0; i < (N*(N-1))/2; i++){
				if( (s->Matches)[i] == Path_List[index+1] ){
					(s->cap_to_matches)[i] -= flow;
					break;
				}
			}
		}

		else if((Path_List[index])->node_type == 'm'){
			if( (Path_List[index])->First_Team == Path_List[index+1] ){
				(Path_List[index])->cap_to_first -= flow;
			}

			else if( (Path_List[index])->Second_Team == Path_List[index+1] ){
				(Path_List[index])->cap_to_second -= flow;
			}
		}

		else if((Path_List[index])->node_type == 't'){
			if( (Path_List[index])->Dest == Path_List[index+1] ){
				(Path_List[index])->cap_to_destination -= flow;
			}

			else {
				for(int i = 0; i < N-1; i++){
					if( ((Path_List[index])->Matchess)[i] == Path_List[index+1] ){
						((Path_List[index])->cap_to_matchess)[i] -= flow;
						break;
					}
				}
			}
		}

		else if((Path_List[index])->node_type == 'd'){
			break;
		}

		index++;
	}


	index--; //	destination(d) directly ignored


	//	Incrementing REVERSE flows
	while(true){
		if((Path_List[index])->node_type == 'm'){
			if( (Path_List[index])->First_Team == Path_List[index-1] ){
				(Path_List[index])->cap_to_first += flow;
			}

			else if( (Path_List[index])->Second_Team == Path_List[index-1] ){
				(Path_List[index])->cap_to_second += flow;
			}
		}

		else if((Path_List[index])->node_type == 't'){
				for(int i = 0; i < N-1; i++){
					if( ((Path_List[index])->Matchess)[i] == Path_List[index-1] ){
						((Path_List[index])->cap_to_matchess)[i] += flow;
						break;
					}
				}
		}

		else if((Path_List[index])->node_type == 's'){
			break;
		}

		index--;
	}
}


void  League::find_teams(int index, int &team1, int &team2){
	int j;
	int sum = 0;
	int counter = 0;
	
	for(j = N-1; j >= 1; j--){
		counter++;
		sum += j;
		if(index < sum){
			break;
		}
	}
	
	team1 = counter - 1;
	team2 = N + index - sum;
}


void  League::connect_all_nodes_and_give_capacities(){

	for(int i = 0; i < (N*(N-1))/2; i++){
		(s->Matches)[i] = &(matches[i]);
		(s->cap_to_matches)[i] = src_flows[i];
		
		(matches[i]).Src = s;
		(matches[i]).cap_to_source = 0;
		
		int t1 = -1;
		int t2 = -1;
		
		find_teams(i, t1, t2);
		
		(matches[i]).First_Team = &(teams[t1]);
		(matches[i]).cap_to_first = maximum;
		
		(matches[i]).Second_Team = &(teams[t2]);
		(matches[i]).cap_to_second = maximum;
	}

	for(int j = 0; j < N; j++){
		(d->Teams)[j] = &(teams[j]);
		(d->cap_to_teams)[j] = 0;

		int tm1, tm2;
		int l = 0;
		for(int k = 0; k < (N*(N-1))/2; k++){
			find_teams(k, tm1, tm2);
			if( tm1 == j  ||  tm2 == j ){
				((teams[j]).Matchess)[l] = &(matches[k]);
				((teams[j]).cap_to_matchess)[l] = 0;
				l++;
			}
		}
		
		(teams[j]).Dest = d;
		(teams[j]).cap_to_destination = dest_flows[j];
	}
}



void  League::print_network(){
	cout << endl << "-------------------------- GAME(" << N << ") --------------------------" << endl;

	int j = 0;

	for(int i = 0; i < (N*(N-1))/2; i++){
		if( i < (N*(N-3))/4 ){
			cout << "\t\t\tMatch[" << i << "] :  " << &(matches[i]) << endl;
		}
		else if( i < ((N*(N-1))/4) - 1 ){
			cout << "\t\t\tMatch[" << i << "] :  " << &(matches[i]) << "\t\tTeams[" << j << "] :  " << &(teams[j]) << endl;
			j++;
		}
		else if( i < (N*(N-1))/4 ){
			cout << "  S : " << s << "\t\tMatch[" << i << "] :  " << &(matches[i]) << "\t\tTeams[" << j << "] :  " << &(teams[j]) << "\t\tD :  " << d << endl;
			j++;
		}
		else if(i < (N*(N+1))/4 ){
			cout << "\t\t\tMatch[" << i << "] :  " << &(matches[i]) << "\t\tTeams[" << j << "] :  " << &(teams[j]) << endl;
			j++;
		}
		else {
			cout << "\t\t\tMatch[" << i << "] :  " << &(matches[i]) << endl;
		}
	}

	cout << "-----------------------------------------------------------------------" << endl << endl;
}



void  League::print_connections(){
	cout << endl << endl << "-------------------------- CONNECTIONS(" << N << ") --------------------------" << endl;
	cout << "\n Connections of Match Nodes:\n" << endl;

	for(int i = 0; i < (N*(N-1))/2; i++){
		cout << "Match[" << i << "] :  " << &(matches[i]) << endl;
		cout << "- - - - - - - - - -\n";
		cout << "First  :  " << matches[i].First_Team << endl;
		cout << "Second :  " << matches[i].Second_Team << endl << endl << endl;
	}

	cout << "\n\n Connections of Team Nodes:\n" << endl;
	for(int j = 0; j < N; j++){
		cout << "Teams[" << j << "] :  " << &(teams[j]) << endl;
		cout << "- - - - - - - - - -\n";

		for(int k = 0; k < N-1; k++){
			cout << "  Matchess[k]:  " << (teams[j]).Matchess[k] << endl;
		}

		cout << endl << endl;

	}
	cout << "-----------------------------------------------------------------------" << endl << endl;
}



bool  League::is_there_any_remaining_source_flow(){
	int lnk;

	for(int i = 0; i < (N*(N-1))/2; i++){
		lnk = (s->cap_to_matches)[i];
		if(lnk > 0){
			return true;
		}
	}
	return false;
}


void  League::make_all_nodes_unvisited(){
	s->visited = false;
	d->visited = false;

	for(int i = 0; i < (N*(N-1))/2; i++){
		(matches[i]).visited = false;
	}

	for(int j = 0; j < N; j++){
		(teams[j]).visited = false;
	}
}


void  League::clear_stack_and_path(){
	for(int i = 0; i < ((N*N) + N + 4)/2; i++){
		Stack[i] = NULL;
		Path_List[i] = NULL;
	}

	top_stack = 0;
	top_path = 0;
}


bool  League::is_stack_empty(){
	return (top_stack == 0);
}


void  League::validate_if_it_is_correct_path(){
	if( (Path_List[0] == s)  &&  (Path_List[top_path-1] == d) ){
		cout << endl << "This path is correct with s at the beginning and d at the end" << endl << endl;
	}
	else {
		cout << endl << "This path is INCORRECT!!" << endl << endl;
	}
}


void  League::print_path(){
	int i = 0;

	cout << endl << endl << " PATH :\t";

	while(true){
		cout << Path_List[i] << ",\t";

		if((Path_List[i])->node_type == 'd'){
			break;
		}

		i++;
	}
	cout << Path_List[i] << endl << endl;

	cout << "Path succesfully printed" << endl << endl << endl;
}


//---------------------------------------------------------------------------------


int main(int argc, char *argv[]){

	if(argc != 2  &&  argc != 3){
		cout << "\n\n Incorrect entry!!\n\n";
		return 0;
	}

	bool out = false;
	char output_file_name[26] = "InputOutput/";

	if(argc == 3){
		out = true;
		strcat(output_file_name, argv[2]);
	}

	char input_file_name[25] = "InputOutput/";
	strcat(input_file_name, argv[1]);

	ifstream  input_file;
	input_file.open(input_file_name);
	
	input_file >> TEAM_NUM;
	
	CURRENT = new int [TEAM_NUM];
	
	for(int i = 0; i < TEAM_NUM; i++){
		input_file >> CURRENT[i];
	}
	cout << endl;

	MATCH_MATRIX = new int* [TEAM_NUM];
	
	for(int j = 0; j < TEAM_NUM; j++){
		MATCH_MATRIX[j] = new int [TEAM_NUM];
	}

	for(int u = 0; u < TEAM_NUM; u++){
		for(int v = 0; v < TEAM_NUM; v++){
			input_file >> MATCH_MATRIX[u][v];
		}
	}
	
	input_file.close();

	int* possibility = new int [TEAM_NUM];


	/* ---------------------------- OBJECT CREATION ----------------------------*/
	for(int i = 0; i < TEAM_NUM; i++){
		League* Soccer = new League (TEAM_NUM, i);
		Soccer->connect_all_nodes_and_give_capacities();

		//	Soccer->print_network();
		//	Soccer->print_connections();
		//	cout << "\n\n Is there any flow?  :   " << Soccer->is_there_any_remaining_source_flow();
		
		possibility[i] = Soccer->IS_IT_POSSIBLE_FOR_TEAM_i_TO_WIN();
		delete Soccer;
	}
	/* -------------------------------------------------------------------------*/

	if(out){
		ofstream  output_file;
		output_file.open(output_file_name);

		for(int j = 0; j < TEAM_NUM; j++){
			output_file << possibility[j] << " ";
		}
		output_file << endl;
		
		output_file.close();
		
		cout << "Solution written to file..." << endl;
	}

	else {
		cout << "Possibilities:" << endl;
		for(int k = 0; k < TEAM_NUM; k++){
			cout << possibility[k] << " ";
		}
	}

	delete [] possibility;

	cout << endl;
	getchar();
	return 0;
}