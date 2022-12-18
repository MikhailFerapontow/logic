#include <iostream>
#include <cmath>
#include <bdd.h>

#include "utils.hpp"

const size_t N = 9;
const size_t M = 4;
const size_t ROW_SIZE = 3;
const int LOG_N = static_cast< int >(std::ceil(std::log2(N)));
constexpr size_t VAR_NUM = 144;
int var[VAR_NUM];

bdd p(int k, int i, int j)
{
  int T = LOG_N;
  int I = T * M * i + T * k;
  
  bdd tree = bddtrue;
  for (int t = 0; t < T; t++)
  {
    tree &= ((j >> t) & 1) ? bdd_ithvar(I + t) : bdd_nithvar(I + t);
  }

  return tree;
}
bdd cond2(int m1, int n1, int m2, int n2);
bdd right_cond3(int m1, int n1, int m2, int n2);
bdd right_bottom_cond3(int m1, int n1, int m2, int n2);
bdd cond4(int m1, int n1, int m2, int n2);
bdd unique();

void fun(char *varset, int size);
void build(char *varset, unsigned n, unsigned I);
void print();

int main()
{
  bdd_init(1000000, 100000);
  bdd_setvarnum(VAR_NUM);
  bdd tree = bddtrue;

  //условие 1-ого рода 6 шт
  tree &= p(1, 0, 0); // 0 объект Париж
  tree &= p(1, 4, 6); // 4 объект Киото
  tree &= p(2, 6, 7); // 6 объект морс
  tree &= p(0, 3, 0); // 3 объект меч
  tree &= p(3, 1, 8); // 1 объект самбреро
  tree &= p(2, 7, 3); // 7 объект водка


  // доп условие
  tree &= p(2,5,2);
  tree &= p(3, 6, 1);
  tree &= p(0,8,3);

  //условие 2-ого рода 8 шт
  tree &= cond2(1, 8, 0, 6); // Тунис - лук
  tree &= cond2(3, 3, 2, 5); // шлем - егермейстер
  tree &= cond2(1, 5, 0, 3); // Каир - рапира
  tree &= cond2(3, 8, 0, 4); // самбреро - катана
  tree &= cond2(2, 1, 0, 1); // бурбон - кинжал
  tree &= cond2(2, 2, 3, 0); // виски - шапка
  tree &= cond2(3, 6, 0, 7); // ушанка - арбалет
  tree &= cond2(0, 5, 2, 7); // копье - морс

  //доп условие 2-ого рода
  // tree &= cond2(1, 6, 0, 2); // киото - топор
  // tree &= cond2(0, 6, 2, 2); // лук - виски
  // tree &= cond2(0, 3, 3, 4); // рапира - цилиндр 
  // tree &= cond2(2, 3, 3, 2); // водка - капюшон
  // tree &= cond2(2, 0, 3, 8); // саке - самбреро
  // tree &= cond2(1, 7, 2, 6); // пекин - сок

  //условие 3-его рода 3 шт
  tree &= right_cond3(0, 2, 2, 2); //справа от топора виски
  tree &= right_cond3(1, 4, 1, 1); //справа от Лондона Москва
  tree &= right_bottom_cond3(3, 5, 0, 2); // справа синзу от каски топор

  //условие 4-ого рода 4 шт
  tree &= cond4(1, 2, 2, 3);// Водка сосед Стамбула
  tree &= cond4(3, 8, 3, 6); // Шапка сосед Самбреро
  tree &= cond4(1, 3, 2, 8); // Настойка сосед Мадрида. c этим считается
  tree &= cond4(0, 0, 1, 6); // Киото сосед меча


  //уникальность
  for (unsigned int i1 = 0; i1 < N - 1; ++i1)
  {
      for (unsigned int i2 = i1 + 1; i2 < N; ++i2)
      {
          for (unsigned int k = 0; k < M; ++k)
          {
              for (unsigned int j = 0; j < N; ++j)
              {
                  tree &= (p(k, i1, j) >> !p(k, i2, j));
              }
          }
      }
  }

  //лишние переменные
  for (unsigned int k = 0; k < M; ++k)
  {
    for (unsigned int i = 0; i < N; ++i)
    {
      for (int j = N; j < std::pow(2, LOG_N); ++j)
      {
        tree &= (!p(k, i, j));
      }
    }
  }

  std::cout << bdd_satcount(tree) << " solution(s):\n";
  bdd_allsat(tree, fun);
  bdd_done();
}

bdd cond2(int m1, int n1, int m2, int n2)
{
  bdd tree = bddtrue;
  for (int i = 0; i < N; i++)
  {
    tree &= p(m1, i, n1) >> p(m2, i, n2);
    tree &= p(m1, i, n1) << p(m2, i, n2);
  }
  return tree;
}

bdd right_cond3(int m1, int n1, int m2, int n2)
{
  bdd tree = bddtrue;
  for (int i = 0; i < N; i++)
  {
    int prev = prev_right(ROW_SIZE , i);
    if (prev == -1)
    {
      tree &= !(p(m2, i, n2));
    }

    int next = next_right(ROW_SIZE, i);
    if (next == -1)
    {
      tree &= !(p(m1, i, n1));
    }
    else
    {
      tree &= p(m1, i, n1) >> p(m2, next, n2);
      tree &= p(m1, i, n1) << p(m2, next, n2);
    }
  }

  return tree;
}

bdd right_bottom_cond3( int m1, int n1, int m2, int n2)
{
  bdd tree = bddtrue;
  for (int i = 0; i < N; i++)
  {
    int prev = prev_right_bottom(ROW_SIZE, i);
    if (prev == -1)
    {
      tree &= !(p(m2, i, n2));
    }

    int next = next_right_bottom(ROW_SIZE, i);
    if (next == -1)
    {
      tree &= !(p(m1, i, n1));
    }
    else
    {
      tree &= p(m1, i, n1) >> p(m2, next, n2);
      tree &= p(m1, i, n1) << p(m2, next, n2);
    }
  }

  return tree;
}

bdd cond4(int m1, int n1, int m2, int n2)
{
  bdd tree = bddtrue;

  for (int i = 0; i < N; i++)
  {
    tree &= right_cond3(m1, n1, m2, n2) | right_bottom_cond3(m1, n1, m2, n2);
  }

  return tree;
}

bdd unique()
{
  bdd tree = bddtrue;
  for (size_t m = 0; m < M; m++)
  {
    for(size_t n = 0; n < N; n++)
    {
      for(size_t i1 = 0; i1 < N - 1; i1++)
      {
        for (size_t i2 = i1 + 1; i2 < N; i2++)
        {
          tree &= p(m, i1, n) >> !(p(m, i2, n));
        }
      }
    }
  }
  return tree;
}

void print()
{
	for (unsigned i = 0; i < N; i++)
	{
		std::cout << i << ": ";
		for (unsigned j = 0; j < M; j++)
		{
			int J = i * M * LOG_N + j * LOG_N;
			int num = 0;
			for (unsigned k = 0; k < LOG_N; k++) num += (unsigned) (var[J + k] << k);
			std::cout << num << ' ';
		}
		std::cout << "\n";
	}
	std::cout << "\n";
}

void build(char *varset, unsigned n, unsigned I)
{
	if (I == n - 1) {
		if (varset[I] >= 0)
		{
			var[I] = varset[I];
			print();
			return;
		}
		var[I] = 0;
		print();
		var[I] = 1;
		print();
		return;
	}
	if (varset[I] >= 0)
	{
		var[I] = varset[I];
		build(varset, n, I + 1);
		return;
	}
	var[I] = 0;
	build(varset, n, I + 1);
	var[I] = 1;
	build(varset, n, I + 1);
}

void fun(char *varset, int size)
{
	build(varset, size, 0);
}