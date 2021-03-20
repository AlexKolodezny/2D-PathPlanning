#include "genetic.h"
#include "searchresult.h"
#include "map.h"
#include "gl_const.h"
#include <list>
#include <optional>
#include <random>
#include <vector>
#include <algorithm>
#include "node.h"
#include "path_utils.h"

const int dx[4] = {1, 0, -1, 0};
const int dy[4] = {0, 1, 0, -1};

int get_direction(std::pair<int, int> first, std::pair<int, int> second) {
    int k = 0;
    while (first.first + dx[k] != second.first || first.second + dy[k] != second.second) {
        ++k;
    }
    return k;
}

double length_between_adjecent_cells(std::pair<int, int> a, std::pair<int, int> b) {
    if (abs(a.first - b.first) == abs(a.second - b.second)) {
        return CN_SQRT_TWO;
    }
    return 1;
}


long long vector_product(std::pair<int, int> a, std::pair<int, int> b, std::pair<int, int> c) {
    return (long long)(a.first - c.first) * (b.second - c.second) - (long long)(a.second - c.second) * (b.first - c.first);
}

//make_line_path_helper make path between start and end included
//require start.first <= end.first && start.second <= end.second
std::list<GeneticNode> make_line_path_helper(std::pair<int, int> start, std::pair<int, int> end) {
    std::list<GeneticNode> res;
    for (std::pair<int, int> cur = start; cur != end; ) {
        res.push_back({cur});
        std::pair<int ,int> p1{2 * cur.first + 2, 2 * cur.second};
        std::pair<int, int> p2{2 * cur.first + 2, 2 * cur.second + 2};
        std::pair<int, int> p3{2 * cur.first, 2 * cur.second + 2};
        std::pair<int, int> begin_line{2 * start.first + 1, 2 * start.second + 1};
        std::pair<int, int> end_line{2 * end.first + 1, 2 * end.second + 1};
        long long v1 = vector_product(end_line, begin_line, p1);
        long long v2 = vector_product(end_line, begin_line, p2);
        if ((v1 <= 0 && v2 >= 0) || (v2 <= 0 && v1 >= 0)) {
            ++cur.first;
        } else {
            ++cur.second;
        }
    }
    res.push_back({end});
    return res;

}

std::list<GeneticNode> GeneticAlgorithm::line_path(std::pair<int, int> start, std::pair<int, int> end) {
    if (start.first > end.first) {
        if (start.second > end.second) {
            auto res = make_line_path_helper(end, start);
            res.reverse();
            res.pop_back();
            return res;
        }
        start.first = -start.first;
        end.first = -end.first;
        auto res = make_line_path_helper(start, end);
        for (auto& cur : res) {
            cur.cell.first = -cur.cell.first;
        }
        res.pop_back();
        return res;
    }
    if (start.second > end.second) {
        start.second = -start.second;
        end.second = -end.second;
        auto res = make_line_path_helper(start, end);
        for (auto& cur : res) {
            cur.cell.second = -cur.cell.second;
        }
        res.pop_back();
        return res;
    }
    auto res = make_line_path_helper(start, end);
    res.pop_back();
    return res;
}

//random_path generates path from start to end not included
//resulting legnth and danger return in the poiner arguments
//the end cell does not included in danger
std::list<GeneticNode> GeneticAlgorithm::random_path(std::pair<int, int> start, std::pair<int, int> end) {
    std::pair<double, double> mean{start.first + end.first, start.second + end.second};
    mean.first /= 2;
    mean.second /= 2;
    std::normal_distribution<double> d{0, static_cast<double>(std::max(std::abs(start.first - end.first), std::abs(start.second - end.second)))};
    std::pair<int, int> pass;
    do {
        pass = {std::round(d(random_generator) + mean.first), std::round(d(random_generator) + mean.second)};
    } while (!grid.CellOnGrid(pass.first, pass.second));
    std::list<GeneticNode> res;
    res.splice(res.end(), line_path(start, pass));
    res.splice(res.end(), line_path(pass, end));
    return res;
}

void GeneticAlgorithm::initialize_length_and_danger(GeneticAlgorithm::Individ& ind) const {
    ind.danger = 0;
    ind.length = 0;
    ind.cross_obstacles = 0;

    for (auto it = ind.path.begin(); it != std::prev(ind.path.end()); ++it) {
        ind.cross_obstacles += grid.CellIsObstacle(it->cell.first, it->cell.second);
        ind.danger += grid.getCellDanger(it->cell.first, it->cell.second);
        ind.length += length_between_adjecent_cells(it->cell, next(it)->cell);
    }
    auto end = std::prev(ind.path.end())->cell;
    ind.danger += grid.getCellDanger(end.first, end.second);
    ind.cross_obstacles += grid.CellIsObstacle(end.first, end.second);
    return;
}

GeneticAlgorithm::Individ::Individ(const GeneticAlgorithm& search, const Individ& parent1, std::list<GeneticNode>::const_iterator it1, const Individ& patent2, std::list<GeneticNode>::const_iterator it2) {
    path.insert(path.end(), parent1.path.begin(), it1);
    path.insert(path.end(), it2, patent2.path.end());
    search.initialize_length_and_danger(*this);
}

GeneticAlgorithm::Individ::Individ(GeneticAlgorithm& search, size_t k): path{} {
    std::pair<int, int> start = search.grid.getStartNode();
    std::uniform_int_distribution<int> along_first_coor{0, search.grid.getMapWidth() - 1};
    std::uniform_int_distribution<int> along_second_coor{0, search.grid.getMapHeight() - 1};
    for (size_t i = 0; i < k; ++i) {
        std::pair<int, int> next{along_first_coor(search.random_generator), along_second_coor(search.random_generator)};
        path.splice(path.end(), search.line_path(start, next));
        start = next;
    }
    std::pair<int, int> end = search.grid.getGoalNode();
    path.splice(path.end(), search.line_path(start, end));
    path.push_back({end});

    search.initialize_length_and_danger(*this);
}
GeneticAlgorithm::Individ GeneticAlgorithm::Individ::copy() const {
    return *this;
}

auto GeneticAlgorithm::crossover(const Individ& parent1, const Individ& parent2) -> std::optional<std::pair<Individ, Individ>> {
    std::unordered_map<std::pair<int, int>,
                        std::list<GeneticNode>::const_iterator,
                        HashCoordinate
                        > cells{10, HashCoordinate{grid.getMapHeight()}};
    size_t this_gap_from_start = 1;
    size_t this_gap_from_end = 1;
    size_t other_gap_from_start = 1;
    size_t other_gap_from_end = 1;

    auto this_search_start = std::next(parent1.path.begin(), this_gap_from_start);
    auto this_search_end = std::prev(parent1.path.begin(), this_gap_from_end);
    auto other_search_start = std::next(parent2.path.begin(), other_gap_from_start);
    auto other_search_end = std::prev(parent2.path.begin(), other_gap_from_end);

    if (this_gap_from_start + this_gap_from_end >= parent1.path.size() || other_gap_from_end + other_gap_from_start > parent2.path.size()) {
        return {};
    }
    for (auto it = this_search_start; it != this_search_end; ++it) {
        cells[it->cell] = it;
    }
    int count = 0;
    for (auto it = other_search_start; it != other_search_end; ++it) {
        if (cells.find(it->cell) != cells.end()) {
            ++count;
        }
    }
    if (count == 0) {
        return {};
    }
    std::uniform_int_distribution<int> d{0, count - 1};
    int num = d(random_generator);
    std::list<GeneticNode>::const_iterator it_this, it_other;
    
    for (auto it = other_search_start; it != other_search_end; ++it) {
        if (cells.find(it->cell) != cells.end()) {
            if (num == 0) {
                it_this = cells[it->cell];
                it_other = it;
                break;
            }
            --num;
        }
    }
    return std::make_optional<std::pair<Individ, Individ>>(
        Individ{*this, parent1, it_this, parent2, it_other},
        Individ{*this, parent2, it_other, parent1, it_this}
    );
}

void GeneticAlgorithm::mutation(Individ& ind) {
    std::uniform_int_distribution<size_t> d{0, ind.path.size() - 1};
    size_t st = d(random_generator);
    size_t fn = d(random_generator);
    
    if (fn < st) {
        std::swap(st, fn);
    }

    auto start_erase = std::next(ind.path.begin(), st);
    auto end_erase = std::next(start_erase, fn - st);

    auto start = start_erase->cell;
    ind.path.erase(start_erase, end_erase);
    ind.path.splice(end_erase, random_path(start, end_erase->cell));

    initialize_length_and_danger(ind);
}

void GeneticAlgorithm::length_refiner(Individ& ind) {
    std::uniform_int_distribution<size_t> d{0, ind.path.size() - 1};
    size_t st = d(random_generator);
    size_t fn = d(random_generator);

    if (fn < st) {
        std::swap(st, fn);
    }

    auto start_erase = std::next(ind.path.begin(), st);
    auto end_erase = std::next(start_erase, fn - st);

    auto start = start_erase->cell;
    ind.path.erase(start_erase, end_erase);
    ind.path.splice(end_erase, line_path(start, end_erase->cell));
    initialize_length_and_danger(ind);

}

void GeneticAlgorithm::remove_cycles(Individ& ind) const {
    std::unordered_map<std::pair<int, int>, std::list<GeneticNode>::iterator, HashCoordinate> cells{10, HashCoordinate(grid.getMapHeight())};
    for (auto it = ind.path.begin(); it != ind.path.end(); ++it) {
        if (cells.find(it->cell) != cells.end()) {
            auto start_erase = cells[it->cell];
            for (auto j = start_erase; j != it; ++j) {
                cells.erase(j->cell);
            }
            ind.path.erase(start_erase, it);
        }
        cells[it->cell] = it;
    }
    
    initialize_length_and_danger(ind);
}

//bypass is used in invalid_refiner
std::list<GeneticNode> bypass(const Map& grid, std::unordered_map<std::pair<int, int>, std::list<GeneticNode>::iterator, HashCoordinate> goals, std::pair<int, int> start, int turn, int dir, int max_danger) {
    std::pair<int, int> cur = start;
    std::list<GeneticNode> res;
    while (true) {
        std::pair<int, int> next;
        int i = 0;
        for (; i < 4; ++i) {
            next = {cur.first + dx[dir], cur.second + dy[dir]};
            if (!grid.CellOnGrid(next.first, next.second)) {
                return {};
            }
            if (!grid.CellIsObstacle(next.first, next.second) && grid.getCellDanger(next.first, next.second) <= max_danger) {
                break;
            }
            dir = (dir + 4 + turn) % 4;
        }
        if (i == 4) {
            return {};
        }
        res.push_back({next});
        if (goals.find(next) != goals.end()) {
            return res;
        }
        if (next == start) {
            return {};
        }
        cur = next;
        dir = (dir + 4 - turn) % 4;
    }
    return {};
}

bool GeneticAlgorithm::invalid_refiner(Individ& ind) {

    std::unordered_map<std::pair<int, int>, std::list<GeneticNode>::iterator, HashCoordinate> cells{10, HashCoordinate(grid.getMapHeight())};

    for (auto it = ind.path.begin(); it != ind.path.end(); ++it) {
        cells[it->cell] = it;
    }

    for (auto it = ind.path.begin(); it != ind.path.end(); ++it) {
        if (grid.CellIsObstacle(it->cell.first, it->cell.second)) {
            auto start = std::prev(it);
            int k = get_direction(start->cell, it->cell);

            std::list<GeneticNode> res;
            
            if (at_random(0.5)) {
                res = bypass(this->grid, cells, start->cell, 1, k, env.dangerlevel);
                if (res.empty()) {
                    res = bypass(this->grid, cells, start->cell, -1, k, env.dangerlevel);
                }
            } else {
                res = bypass(this->grid, cells, start->cell, -1, k, env.dangerlevel);
                if (res.empty()) {
                    res = bypass(this->grid, cells, start->cell, 1, k, env.dangerlevel);
                }
            }

            if (res.empty()) {
                initialize_length_and_danger(ind);
                return false;
            }

            auto end_erase = cells[res.back().cell];
            for (auto i = it; i != end_erase; ++i) {
                cells.erase(i->cell);
            }
            it = ind.path.erase(it, end_erase);
            res.pop_back();
            ind.path.splice(it, res);
        }
        cells.erase(it->cell);
    }

    initialize_length_and_danger(ind);
    return true;
}

void GeneticAlgorithm::danger_refiner(Individ& ind) {
    std::uniform_int_distribution<size_t> d{0, ind.path.size() - 1};
    size_t st = d(random_generator);
    size_t fn = d(random_generator);

    if (fn < st) {
        std::swap(st, fn);
    }

    auto start = std::next(ind.path.begin(), st);
    auto end = std::next(ind.path.begin(), fn);

    int k = std::max(grid.getCellDanger(start->cell.first, start->cell.second), grid.getCellDanger(end->cell.first, end->cell.second));
    
    if (k == env.dangerlevel - 1) {
        return;
    }

    std::unordered_map<std::pair<int, int>, std::list<GeneticNode>::iterator, HashCoordinate> cells{10, HashCoordinate(grid.getMapHeight())};

    for (auto cur = end; cur != start; --cur) {
        cells[cur->cell] = cur;
    }

    for (auto it = std::next(start); it != end; ++it) {
        if (grid.getCellDanger(it->cell.first, it->cell.second) > k) {
            auto pr = std::prev(it);
            int dir = get_direction(pr->cell, it->cell);
            int turn = at_random(0.5) ? 1 : -1;
            std::list<GeneticNode> res = bypass(this->grid, cells, pr->cell, turn, dir, k);
            if (res.empty()) {
                return;
            }

            auto end_erase = cells[res.back().cell];
            for (auto i = it; i != end_erase; ++i) {
                cells.erase(i->cell);
            }

            it = ind.path.erase(it, end_erase);
            res.pop_back();
            ind.path.splice(end_erase, res);
        }
        cells.erase(it->cell);
    }

    initialize_length_and_danger(ind);
    return;
}

double GeneticAlgorithm::Individ::get_path_length() const {
    return length;
}

double GeneticAlgorithm::Individ::get_path_danger() const {
    return danger;
}

bool GeneticAlgorithm::Individ::is_invalid() const {
    return cross_obstacles != 0;
}

int GeneticAlgorithm::Individ::get_crossed_obstacles() const {
    return cross_obstacles;
}

std::list<Node> GeneticAlgorithm::get_result(const Individ& ind) const {
    std::list<Node> res;
    res.emplace_back(ind.path.front().cell.first, ind.path.front().cell.second, 0, 0, 0, nullptr);
    for (auto it = std::next(ind.path.begin()); it != ind.path.end(); ++it) {
        Node node{
            it->cell.first,
            it->cell.second, 
            0, 
            res.back().g1 + length_between_adjecent_cells(it->cell, {res.back().i, res.back().j}),
            res.back().g2 + grid.getCellDanger(it->cell.first, it->cell.second),
            &res.back()
        };
        res.push_back(node);
    }
    return res;
}

//Algorithm NSGA_II, sort the generation
void GeneticAlgorithm::nsga_ii(std::vector<Individ>& generation) {
    auto part = std::partition(generation.begin(), generation.end(), 
        [](const Individ& a) {return !a.is_invalid();});
    std::sort(generation.begin(), part, 
        [](const Individ& a, const Individ& b) {return a.get_path_length() < b.get_path_length();});
    std::sort(part, generation.end(), 
        [](const Individ& a, const Individ& b) {
            return a.get_crossed_obstacles() < b.get_crossed_obstacles();
        });
    std::vector<std::vector<std::pair<Individ, double>>> fronts;
    for (auto it = generation.begin(); it != part; ++it) {
        auto front = std::partition_point(fronts.begin(), fronts.end(),
            [&it](const std::vector<std::pair<Individ, double>>& a) {
                return a.back().first.get_path_danger() <= it->get_path_danger();
            });
        if (front != fronts.end()) {
            front->emplace_back(std::move(*it), 0);
        } else {
            fronts.emplace_back();
            fronts.back().emplace_back(std::move(*it), 0);
        }
    }

    auto it = generation.begin();

    for (auto& ft : fronts) {
        double min_length = ft.front().first.get_path_length();
        double max_length = ft.back().first.get_path_length();
        double max_danger = ft.front().first.get_path_danger();
        double min_danger = ft.back().first.get_path_danger();

        //set first and last elements to be greater than each other
        ft.front().second = 3;
        ft.back().second = 3;

        for (size_t i = 1; i + 1 < ft.size(); ++i) {
            ft[i].second = (ft[i + 1].first.get_path_length() - ft[i - 1].first.get_path_length()) / (max_length - min_length);
            ft[i].second += (ft[i - 1].first.get_path_danger() - ft[i + 1].first.get_path_danger()) / (max_danger - min_danger);
        }

        std::sort(ft.begin(), ft.end(),
            [](const std::pair<Individ, double>& a, const std::pair<Individ, double>& b) {
                return a.second > b.second;
            });

        for (auto& x : ft) {
            *it = std::move(x.first);
            ++it;
        }
    }

    return;
}

GeneticAlgorithm::Individ* GeneticAlgorithm::choose_parent(std::vector<Individ>& v) {
    std::uniform_int_distribution<size_t> d{0, v.size() - 1};
    size_t x = d(random_generator);
    size_t y = d(random_generator);
    return &v[std::min(x, y)];
    
}

GeneticAlgorithm::GeneticAlgorithm(const Map& grid, const EnvironmentOptions& env)
    : grid{grid}
    , env{env} {}

GeneticAlgorithm::~GeneticAlgorithm() {}

bool GeneticAlgorithm::at_random(double p) {
    std::bernoulli_distribution d{p};
    return d(random_generator);
}

SearchResult GeneticAlgorithm::startSearch(ILogger *, const Map &, const EnvironmentOptions &) {
    
    size_t generation_size = 50;
    size_t epoch_number = 100;
    size_t child_number = 50;
    size_t k = 50;

    double p_mutaion = 0.1;
    double p_length_refine = 0.5;
    double p_danger_refine = 0.5;


    std::vector<Individ> generation;
    std::generate_n(std::back_inserter(generation), generation_size, [this, k](){
        return Individ{*this, k};
    });

    for (auto& x : generation) {
        remove_cycles(x);
    }


    nsga_ii(generation);

    for (size_t epoch = 0; epoch < epoch_number; ++epoch) {
        std::vector<Individ> childs;
        for (size_t i = 0; i < child_number;) {
            auto p1 = choose_parent(generation);
            auto p2 = choose_parent(generation);
            if (p1 != p2) {
                double p = p_mutaion;
                auto couple = crossover(*p1, *p2);
                if (!couple) {
                    couple.emplace(std::pair<Individ, Individ>{p1->copy(), p2->copy()});
                    p = 1;
                }
                if (at_random(p)) {
                    mutation(couple.value().first);
                }
                if (at_random(p)) {
                    mutation(couple.value().second);
                }
                childs.push_back(std::move(couple.value().first));
                childs.push_back(std::move(couple.value().second));
                i += 2;
            }
        }
        for (auto& c : childs) {
            if (at_random(p_length_refine)) {
                length_refiner(c);
            }
            if (at_random(p_danger_refine)) {
                danger_refiner(c);
            }
        }
        if (epoch * 4 >= epoch_number) {
            for (auto& c : childs) {
                invalid_refiner(c);
            }
        }
        for (auto& c : childs) {
            remove_cycles(c);
        }
        std::move(childs.begin(), childs.end(), std::back_inserter(generation));
        nsga_ii(generation);
        generation.erase(generation.begin() + generation_size, generation.end());
    }

    
    for (auto& ind : generation) {
        sresult.lppaths.push_back(get_result(ind));
        sresult.hppaths.push_back(make_secondary_path(sresult.lppaths.back()));
        sresult.pathlength.push_back(ind.get_path_length());
    }

    sresult.pathfound = !sresult.lppaths.empty();

    sresult.nodescreated = 0;
    sresult.numberofsteps = 0;
    sresult.time = 0;
    return sresult;
}
