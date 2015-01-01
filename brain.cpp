#include "stdafx.h";
int g_s[8][9] = {
	{0,1,2,3,4,5,6,7,8},
	{6,3,0,7,4,1,8,5,2},
	{8,7,6,5,4,3,2,1,0},
	{2,5,8,1,4,7,0,3,6},
	{6,7,8,3,4,5,0,1,2},
	{2,1,0,5,4,3,8,7,6},
	{8,5,2,7,4,1,6,3,0},
	{0,3,6,1,4,7,2,5,0}
};
int g_indexes[8][3] = { // множество линий для анализа побед
	{0,1,2}, {3,4,5}, {6,7,8}, {0,3,6},	{1,4,7}, {2,5,8}, {0,4,8}, {2,4,6}
};
int Brain::FindPattern()
{
	// определить какая матрица используется
	// признак кто ходил первым 0 - компьютер, 1 - человек
	// строка ходов, база данных
	std::vector<NERV_CELL>::iterator c;
	std::vector<int> sim;
	int f1[9] = {0}, f2[9];
	for (int i = 0; i < 9; i++)
	{
		f1[i] = (BYTE)m_field[i];
	}
	for(c = mind_win.begin(); c != mind_win.end(); c++)
	{
		RtlZeroMemory(f2, sizeof(f2));
		for (int i = 0; i < (int)m_nerv.cnt; i++)
		{
			f2[(*c).field[i]] = (i+(*c).first+1)%2+1;
		}
		
		sim.clear();
			
		if (Cmp(f1, f2, sim) > 0)
		{
			// нашли симметрии
								// с учетом симметрии выбираем ход
			int k = 0;
			if (sim.size()>1) k = rand() % sim.size();
			int p = (*c).field[m_nerv.cnt];
			int v = g_s[sim[k]][p];
			return v;
				
		}
		
	}
	for(c = mind_par.begin(); c != mind_par.end(); c++)
	{
		RtlZeroMemory(f2, sizeof(f2));
		for (int i = 0; i < (int)m_nerv.cnt; i++)
		{
			f2[(*c).field[i]] = (i+(*c).first+1)%2+1;
		}
		
		sim.clear();			
		if (Cmp(f1, f2, sim) > 0)
		{
		// нашли симметрии
			
			// с учетом симметрии выбираем ход
			int k = 0;
			if (sim.size()>1) k = rand() % sim.size();
			int p = (*c).field[m_nerv.cnt];
			int v = g_s[sim[k]][p];
			return v;
		}
	}
	return -1;
}
int Brain::Cmp(int *pattern, int *in, std::vector<int> &sim)
{
	
	
	int cnt = 0;
	// прямое сравнение
	for (int i = 0; i < 8; i++)
	{
		bool result = true;
		for(int j = 0; j < 9; j++)
		{
			if (in[g_s[i][j]]!=pattern[j]) {
				result = false; break;
			}
		}
		if (result) {
			cnt++;
			sim.push_back(i);
		}
	}
	
	return cnt;
}
Brain::Brain(TCHAR *name)
{
	FILE *fb;
	NERV_CELL tmp;
	errno_t error = _tfopen_s(&fb,name, _T("rb"));
	
	
	memcpy(fname, name, sizeof(TCHAR) * (_tcsclen(name)+1));
	if (error>0) return;
	// загрузить мозги
	while (fread_s(&tmp, sizeof(NERV_CELL), sizeof(NERV_CELL),1, fb))
	{
		if (tmp.result==RES_WIN)
			mind_win.push_back(tmp);
		else
			mind_par.push_back(tmp);
	}
	fclose(fb);
}
void Brain::Learn(int result)
{
	NERV_CELL tmp;
	if (m_needLearn){
		tmp.cnt = m_nerv.cnt;
		memcpy(tmp.field, m_nerv.field, 9);
		if (result==PARITY){
			tmp.result = RES_PARITY;
			mind_par.push_back(tmp);
		}else {
			if (result==COMPUTER_WIN){
				tmp.first = (m_first?1:0);
			}else
				tmp.first = (m_first?0:1);
			tmp.result = RES_WIN;
			mind_win.push_back(tmp);
		}		
	}	
}
void Brain::InitGame(bool first, int *field)
{
	m_field = field;
	m_first = first;
	m_who = first;
	m_inProgress = true;
	m_needLearn = false;
	RtlZeroMemory(&m_nerv, sizeof(NERV_INPUT));

}
void Brain::MoveUser(int w)
{
	m_field[w] = 2;
	m_nerv.field[m_nerv.cnt++] = w;
	m_who = !m_who;
}
bool Brain::CanMove()
{
	return m_inProgress && !m_who;
}
void Brain::Move()
{
// Мозги здесь!!!
	// сначала нападаем
	int i,j;
	for (i = 0; i < 8; i++)
	{
		// найти ряд, где одно пустое поле и два вражеских, 
		int comp = 0, empty = 0, iempty = 0;
		for (j = 0; j < 3; j++)
		{
			if (m_field[g_indexes[i][j]] == 1)
				comp++;
			if (m_field[g_indexes[i][j]] == 0)
			{
				empty++;
				iempty = g_indexes[i][j];
			}
		}
		// правило немедленного нападения
		if (empty==1 && comp==2)
		{
			// а вдруг полей несколько?
			// пока ходим по очевидному
			m_field[iempty] = 1;
			m_nerv.field[m_nerv.cnt++] = iempty;
			m_who = !m_who;
			return;
		}
	}
	// Анализ защиты
	for (i = 0; i < 8; i++)
	{
		// найти ряд, где одно пустое поле и два вражеских, 
		int enemy = 0, empty = 0, iempty = 0;
		for (j = 0; j < 3; j++)
		{
			if (m_field[g_indexes[i][j]] == 2)
				enemy++;
			if (m_field[g_indexes[i][j]] == 0)
			{
				empty++;
				iempty = g_indexes[i][j];
			}
		}
		// правило немедленной защиты
		if (empty==1 && enemy==2)
		{
			// а вдруг полей несколько?
			// пока ходим по очевидному
			m_field[iempty] = 1;
			m_nerv.field[m_nerv.cnt++] = iempty;
			m_who = !m_who;
			return;
		}
	}
	// занят ли центр
	if (!m_field[4])
	{
		m_field[4] = 1;
		m_nerv.field[m_nerv.cnt++] = 4;
		m_who = !m_who;
		return;
	}
	// или рандомный ход
	// самообучающаяся часть
	// загрузить мозги
	int move;
	if ((move = FindPattern())==-1)
	{
		m_needLearn = true;
		// случайный ход
		j = rand()%2;
		
		while(m_field[j % 9])j+=2;
		move = j % 9;		
	}

	m_field[move] = 1;
	m_nerv.field[m_nerv.cnt++] = move;
	m_who = !m_who;	
}
int Brain::AnalyseGame()
{
	for(int i = 0; i < 8;i++){
		int c = 0, u = 0;
		for (int j = 0; j < 3; j++){
			if (m_field[g_indexes[i][j]]==1) c++;
			else if (m_field[g_indexes[i][j]]==2) u++;
		}
		if (c==3) { m_inProgress = false; return COMPUTER_WIN;}
		if (u==3) { m_inProgress = false; return USER_WIN;}
	}
	if (m_nerv.cnt==9){
		m_inProgress = false;
		return PARITY;
	}
	return IN_PROGRESS;
}
Brain::~Brain()
{
	FILE *fb;
	errno_t error = _tfopen_s(&fb,fname, _T("wb+"));
	if (!error){
		for(std::vector<NERV_CELL>::iterator c = mind_win.begin(); c != mind_win.end(); c++)
		{
			fwrite(&(*c),sizeof(NERV_CELL), 1, fb);
		}
		for(std::vector<NERV_CELL>::iterator c = mind_par.begin(); c != mind_par.end(); c++)
		{
			fwrite(&(*c),sizeof(NERV_CELL), 1, fb);
		}
		fclose(fb);
	}
}