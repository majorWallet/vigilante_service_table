#include <iostream>
#include <fstream>
#include <algorithm>
#include <string>
#include <vector>
#include <random>
using namespace std;

const int ammunition = 8;  //탄약고 근무시간
const int guardroom = 3;  //후문 근무시간
const int vigil = 2;  //불침번 근무시간
const int onDuty = 1;  //당직사관 근무시간

/*
	1. 본 근무표는 중대 단위로 2주 마다 작성된다.
	2. 탄약고->후문->탄약고->후문...(5대기 소대는 알아서 정한다.)
	3. 탄약고 근무는 하루에 12명을 뽑으며 3교대이다.
	4. 탄약고 근무는 한개 타임에 4명이며 30분 씩 교대이다.
	5. 퐁당퐁당은 가급적 피할것.
	6. 최소 중대 인원은 12(탄약고) + 5(불침번) + 1(당직병) = 18명 이다.
	7. 당직병은 열외이다.
	8. 추후 병사 정보에 소속 소대도 추가할 예정
*/

class Serviceman {
private:
	string rank = "???";  //계급
	string name = "???";  //이름
	bool chkUsable = true;  //열외 유무 (false => 열외)
	string inferiority_reason = "???"; //열외 사유
	int workingHr = 0;
public:
	Serviceman(string _rank = "???", string _name = "???")
		:rank(_rank), name(_name) {

	}

	virtual string getRank() const {
		return rank;
	}

	virtual string getName() const {
		return name;
	}

	virtual int getHr() const {
		return workingHr;
	}

	virtual bool chk_inf() const {
		return chkUsable;
	}

	virtual void inferiority() {
		chkUsable = false;
	}

	virtual void doWork() {
		chkUsable = true;
	}

	virtual void plusHr(int num) {
		workingHr += num;
	}

	virtual void printInfo() const {
		cout << this->getRank() << '\t' << this->getName() << '\t' << this->getID() << '\t';
		if (this->chk_inf())
			cout << "가용" << endl;
		else
			cout << "열외" << endl;
	}

	virtual string getID() const = 0;
};

class Cadre : public Serviceman {
	//간부 클래스
	//당직사관(중대 단위 경작서이므로 당직사령은 제외)
private:
	string ID = "??-??????";  //군번
public:
	Cadre(string _rank = "???", string _name = "???", string _ID = "??-??????")
		:Serviceman(_rank, _name), ID(_ID) {

	}

	virtual string getID() const override {
		return ID;
	}
};

class Soldier : public Serviceman {
	//병사 클래스
	//탄약고, 5대기, 위병소, 불침번
private:
	string ID = "??-????????";  //군번
public:
	Soldier(string _rank = "???", string _name = "???", string _ID = "??-????????")
		:Serviceman(_rank, _name), ID(_ID) {

	}

	virtual string getID() const override {
		return ID;
	}
};

vector<Serviceman*> getMinList(vector<Serviceman*>& vec) {
	int smallest = vec.at(0)->getHr();
	vector<Serviceman*> minList;

	for (int i = 1; i < vec.size(); ++i)
		smallest = min(smallest, vec.at(i)->getHr());

	for (int i = 0; i < vec.size(); i++)
		if (smallest == vec.at(i)->getHr())
			minList.push_back(vec.at(i));

	return minList;
}

class SoldierHandler {
private:
	vector<Serviceman*> cadreList, soldierList;
	string** workSch = NULL;
public:
	SoldierHandler() { }

	void readFile() {
		string rank, name, ID, inferiority;

		ifstream fin1("cadreList.txt");
		if (!fin1) {
			cout << "textFile Error!!" << endl;
			cout << "Program Exit!!" << endl;
			exit();
		}

		ifstream fin2("soldierList.txt");
		if (!fin2) {
			cout << "textFile Error!!" << endl;
			cout << "Program Exit!!" << endl;
			exit();
		}

		while (fin1 >> rank >> name >> ID >> inferiority) {
			cadreList.push_back(new Cadre(rank, name, ID));
			if (inferiority == "열외")
				cadreList.back()->inferiority();
			else if (inferiority == "가용")
				cadreList.back()->doWork();
			else {
				cout << "textFile Error!!" << endl;
				cout << "Program Exit!!" << endl;
				exit();
			}
		}

		while (fin2 >> rank >> name >> ID >> inferiority) {
			soldierList.push_back(new Soldier(rank, name, ID));
			if (inferiority == "열외")
				soldierList.back()->inferiority();
			else if (inferiority == "가용")
				soldierList.back()->doWork();
			else {
				cout << "textFile Error!!" << endl;
				cout << "Program Exit!!" << endl;
				exit();
			}
		}

		fin1.close();
		fin2.close();
	}

	int chkID(string& ID) {
		if (ID[2] == '-' && (ID.length() == 9 || ID.length() == 11)) {
			if (ID.length() == 9) {
				for (int i = 0; i < cadreList.size(); i++)
					if (ID == cadreList.at(i)->getID())
						return i;
			}
			
			else if (ID.length() == 11) {
				for (int i = 0; i < soldierList.size(); i++)
					if (ID == soldierList.at(i)->getID())
						return i;
			}
					
			return -1;  //-1반환 -> 존재하지 않은 군번이 입력된 경우
		}

		else {
			cout << "군번이 올바르지 않습니다." << endl;
			return -2;  // -2반환 -> 올바르지 않은 군번이 입력된 경우
		}
	}

	void showMenu() const {
		cout << "---------Menu---------" << endl;
		cout << "1. 전입" << endl;
		cout << "2. 전역" << endl;
		cout << "3. 열외" << endl;
		cout << "4. 근무 투입" << endl;
		cout << "5. 전병력 출력" << endl;
		cout << "6. 경계작전명령서 생성" << endl;
		cout << "7. 프로그램 종료" << endl;
	}

	void enlistment() {
		int sel;
		string rank, name, ID;

		do {
			cout << "   [선택]   " << endl;
			cout << "1. 간부 전입" << endl;
			cout << "2. 병사 전입" << endl;
			cout << "3. 취소" << endl;
			cout << "선택: ";
			cin >> sel;
			cout << endl;
		} while (sel != 1 && sel != 2 && sel != 3);

		if (sel == 1) {
			cout << "[간부 전입]" << endl;
			cout << "계급: ";
			cin >> rank;
			cout << "이름: ";
			cin >> name;
			cout << "군번: ";
			cin >> ID;
			cout << endl;
			if (ID.length() == 9) {
				if (chkID(ID) == -1)
					cadreList.push_back(new Cadre(rank, name, ID));
				else if (chkID(ID) >= 0)
					cout << "이미 존재하는 군번입니다." << endl << endl;
			}

			else {
				cout << "군번이 올바르지 않습니다." << endl << endl;
				return;
			}
		}

		else if (sel == 2) {
			cout << "[병사 전입]" << endl;
			cout << "계급: ";
			cin >> rank;
			cout << "이름: ";
			cin >> name;
			cout << "군번: ";
			cin >> ID;
			cout << endl;
			if (ID.length() == 11) {
				if (chkID(ID) == -1)
					soldierList.push_back(new Soldier(rank, name, ID));
				else if (chkID(ID) >= 0)
					cout << "이미 존재하는 군번입니다." << endl << endl;
			}

			else {
				cout << "군번이 올바르지 않습니다." << endl << endl;
				return;
			}
		}

		else if (sel == 3) {
			return;
		}
	}

	void discharge() {
		string ID;
		cout << "[전역]" << endl;
		cout << "취소 시엔 전역자 군번에 X를 입력하시오." << endl;
		cout << "전역자 군번: ";
		cin >> ID;
		cout << endl;

		if (ID.length() == 9) {  //간부
			if (chkID(ID) >= 0)
				cadreList.erase(cadreList.begin() + chkID(ID));
		}

		else if (ID.length() == 11) {  //병사
			if (chkID(ID) >= 0)
				soldierList.erase(soldierList.begin() + chkID(ID));
		}

		else
			return;
	}

	void inferiority() {
		string ID;
		cout << "[열외]" << endl;
		cout << "취소 시엔 열외자 군번에 X를 입력하시오." << endl;
		cout << "열외자 군번: ";
		cin >> ID;
		cout << endl;

		if (ID.length() == 9) {  //간부
			if (chkID(ID) >= 0)
				cadreList.at(chkID(ID))->inferiority();
		}

		else if (ID.length() == 11) {  //병사
			if (chkID(ID) >= 0)
				soldierList.at(chkID(ID))->inferiority();
		}

		else
			return;
	}

	void doWork() {
		string ID;
		cout << "[근무 투입]" << endl;
		cout << "취소 시엔 투입자 군번에 X를 입력하시오." << endl;
		cout << "근무 투입자 군번: ";
		cin >> ID;
		cout << endl;

		if (ID.length() == 9) {  //간부
			if (chkID(ID) >= 0)
				cadreList.at(chkID(ID))->doWork();
		}

		else if (ID.length() == 11) {  //병사
			if (chkID(ID) >= 0)
				soldierList.at(chkID(ID))->doWork();
		}

		else
			return;
	}

	void printSoldiers() const {
		cout << "[간부 명단]" << endl;
		cout << "계급\t성명\t군번\t\t열외유무" << endl;
		for (int i = 0; i < cadreList.size(); i++)
			cadreList.at(i)->printInfo();
		cout << endl;

		cout << "[병사 명단]" << endl;
		cout << "계급\t성명\t군번\t\t열외유무" << endl;
		for (int i = 0; i < soldierList.size(); i++)
			soldierList.at(i)->printInfo();
		cout << endl;
	}

	void makeTable() {
		random_device rd;
		mt19937 gen(rd());

		int day;
		int work;
		int selected_soldier;
		cout << "총 근무 일수를 입력하시오." << endl;
		cout << "입력: ";
		cin >> day;
		cout << "근무 형태를 입력하시오." << endl;
		cout << "1. 탄약고" << endl;
		cout << "2. 후문" << endl;
		cin >> work;
		cout << endl;

		workSch = new string* [day];
		if (work == 1) {
			for (int i = 0; i < day; i++)
				workSch[i] = new string[17];  //1타 4명, 2타 4명, 3타 4명, 불침번 5명 -> 17명

			for (int i = 0; i < day; i++) {
				for (int j = 0; j < 17; j++) {
					vector<Serviceman*> workerList = getMinList(soldierList);  //근무 투입 시간이 가장 적은 병사를 골라 리스트 생성
					uniform_int_distribution<int> dis(0, workerList.size() - 1);  //생성한 리스트 범위만큼 난수 범위 설정
					selected_soldier = dis(gen);
					workSch[i][j] = workerList.at(selected_soldier)->getName();  //발생한 난수를 기반으로 투입 시간이 가장 적은 병사 중 1명을 선택하여 스케줄에 작성
					workerList.at(selected_soldier)->plusHr(ammunition);
				}  //이를 17번 반복 (1일 분량 경작서 완성)
			}  //이를 day 만큼 반복 (day 분량 경작서 완성)
			for (int i = 0; i < 17; i++) {
				for (int j = 0; j < day; j++) {
					cout << workSch[j][i] << '\t';
				}
				cout << endl;
			}
		}

		else if (work == 2) {
			for (int i = 0; i < day; i++)
				workSch[i] = new string[13];  //1타 2명, 2타 2명, 3타 2명, 4타 2명, 불침번 5명 -> 13명

			for (int i = 0; i < day; i++) {
				for (int j = 0; j < 13; j++) {
					vector<Serviceman*> workerList = getMinList(soldierList);  //근무 투입 시간이 가장 적은 병사를 골라 리스트 생성
					uniform_int_distribution<int> dis(0, workerList.size() - 1);  //생성한 리스트 범위만큼 난수 범위 설정
					selected_soldier = dis(gen);
					workSch[i][j] = workerList.at(selected_soldier)->getName();  //발생한 난수를 기반으로 투입 시간이 가장 적은 병사 중 1명을 선택하여 스케줄에 작성
					workerList.at(selected_soldier)->plusHr(ammunition);
				}  //이를 17번 반복 (1일 분량 경작서 완성)
			}  //이를 day 만큼 반복 (day 분량 경작서 완성)
			for (int i = 0; i < 17; i++) {
				for (int j = 0; j < day; j++) {
					cout << workSch[j][i] << '\t';
				}
				cout << endl;
			}
		}
	}

	void exit() const {
		ofstream fout1("cadreList.txt");
		ofstream fout2("soldierList.txt");

		for (int i = 0; i < cadreList.size(); i++) {
			fout1 << cadreList.at(i)->getRank() << '\t' << cadreList.at(i)->getName() << '\t' << cadreList.at(i)->getID() << '\t';
			if (cadreList.at(i)->chk_inf())
				fout1 << "가용" << endl;
			else
				fout1 << "열외" << endl;
		}

		for (int i = 0; i < soldierList.size(); i++) {
			fout2 << soldierList.at(i)->getRank() << '\t' << soldierList.at(i)->getName() << '\t' << soldierList.at(i)->getID() << '\t';
			if (soldierList.at(i)->chk_inf())
				fout2 << "가용" << endl;
			else
				fout2 << "열외" << endl;
		}

		fout1.close();
		fout2.close();
	}
};

int main() {
	SoldierHandler handle;
	int sel;
	handle.readFile();

	while (true) {
		handle.showMenu();
		cout << "선택: ";
		cin >> sel;
		cout << endl;

		if (sel == 1) {
			handle.enlistment();
		}
		else if (sel == 2) {
			handle.discharge();
		}
		else if (sel == 3) {
			handle.inferiority();
		}
		else if (sel == 4) {
			handle.doWork();
		}
		else if (sel == 5) {
			handle.printSoldiers();
		}
		else if (sel == 6) {
			handle.makeTable();
		}
		else if (sel == 7) {
			handle.exit();
			return 0;
		}
	}

	return 0;
}