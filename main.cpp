#include <iostream>
#include <cmath>
#include <bdd.h>

#include "utils.hpp"

const size_t N = 9;
const size_t M = 4;
const size_t ROW_SIZE = 3;
const int LOG_N = static_cast< int >(std::ceil(std::log2(N)));
constexpr size_t VAR_NUM = N * M * LOG_N;// 144;
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

  //условие первого типа 6 шт
  tree &= p(1, 0, 0); // 0 объект город париж
  tree &= p(1, 3, 7); // 3 объект город Пекин
  tree &= p(3, 7, 5); // объек 7 носит каску
  tree &= p(2, 5, 2); // объект 5 пьет виски
  tree &= p(0, 4, 2); // объект 4 носит топор
  tree &= p(0, 6, 5); // объект 6 носит копье

  //условие второго типа 8 шт
  tree &= cond2(1, 2, 3, 7); // кто живет в Стамбуле тот носит колпак
  tree &= cond2(0, 3, 2, 8); // там где носят рапиру пьют настойку
  tree &= cond2(3, 7, 3, 1); // там где пьют морс носят колпак
  tree &= cond2(1, 8, 0, 6); // в Тунисе носят лук
  tree &= cond2(1, 4, 3, 8); // в Лондоне носят самбреро
  tree &= cond2(0, 7, 2, 4); // где носят арбалет пьют воду
  tree &= cond2(1, 6, 3, 3); // в Киото носят шлем
  tree &= cond2(2, 1, , 0, 1); // где пьют бурбон носят кинжал

  //условие третьего типа 3 шт
  tree &= right_bottom_cond3(2, 5, 1, 5); // справа снизу от пьющего егермейстер Каир
  tree &= right_cond3(3, 2, 0, 2); // справа от кепки топор
  tree &= right_bottom_cond3(0, 1, 1, 8); // справа снизу от катаны Тунис

  //условие чевтертого типа 4 шт
  tree &= cond4(1, 6, 3, 4); // рядом с Киото носят цилиндр
  tree &= cond4(3, 5, 2, 0); // рядом с носящим каску пьют саке
  tree &= cond4(2, 3, 0, 2); // рядом с водкой рапира
  tree &= cond4(1, 2, 0, 8); //

  tree &= unique();
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
      tree &= p(m1, i, n1) >> p(m2, i, n2);
      tree &= p(m1, i, n1) << p(m2, i, n2);
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
      tree &= p(m1, i, n1) >> p(m2, i, n2);
      tree &= p(m1, i, n1) << p(m2, i, n2);
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