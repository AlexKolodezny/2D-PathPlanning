#ifndef GENETIC_H
#define GENETIC_H

#include "search.h"
#include "map.h"
#include "searchresult.h"
#include "environmentoptions.h"
#include "ilogger.h"
#include "cell.h"
#include <random>
#include <optional>

struct GeneticNode : public Cell {
};

class GeneticAlgorithm : public Search
{
    const Map& grid;
    std::mt19937 random_generator;
    const EnvironmentOptions& env;
    std::unique_ptr<DangerObjective> obj;

public:
    class Individ;

private:
    void initialize_length_and_danger(Individ& ind) const;
    void length_refiner(Individ& ind);
    void danger_refiner(Individ& ind);
    void mutation(Individ& ind);
    void remove_cycles(Individ& ind) const;
    bool invalid_refiner(Individ& ind);
    void astar_length_refiner(Individ& ind);
    void astar_danger_refiner(Individ& ind);
    std::optional<std::pair<Individ, Individ>> crossover(const Individ& parent1, const Individ& parent2);
    std::list<Cell> get_result(const Individ& ind) const;

    std::list<GeneticNode> line_path(Cell start, Cell end);
    std::list<GeneticNode> random_path(Cell start, Cell end);
    std::list<GeneticNode> astar_path(Cell start, Cell end, int min_danger);

    Individ* choose_parent(std::vector<Individ>& generation);
    std::vector<std::vector<Individ>> nsga_ii(std::vector<Individ>&&);
    bool at_random(double p);

public:
    class Individ {
        std::list<GeneticNode> path;
        double length;
        double danger;
        int cross_obstacles;
        double crowdness;

        Individ(const Individ& parent1, std::list<GeneticNode>::const_iterator it1, const Individ& patent2, std::list<GeneticNode>::const_iterator it2);

        Individ& operator=(const Individ&) = default;
        Individ(const Individ&) = default;
    public:
        Individ(GeneticAlgorithm& search, size_t k);

        Individ(Individ&&) = default;
        Individ& operator=(Individ&&) = default;

        Individ copy() const;

        inline void set_crowdness(double x) {
            crowdness = x;
            return;
        }
        inline double get_crowdness() const {
            return crowdness;
        }

        double get_path_length() const;
        double get_path_danger() const;
        bool is_invalid() const;
        int get_crossed_obstacles() const;
        bool is_bad(Cell start, Cell end) const;

        friend void GeneticAlgorithm::initialize_length_and_danger(Individ& ind) const;
        friend void GeneticAlgorithm::length_refiner(Individ& ind);
        friend void GeneticAlgorithm::danger_refiner(Individ& ind);
        friend void GeneticAlgorithm::mutation(Individ& ind);
        friend void GeneticAlgorithm::remove_cycles(Individ& ind) const;
        friend bool GeneticAlgorithm::invalid_refiner(Individ& ind);
        friend void GeneticAlgorithm::astar_length_refiner(Individ& ind);
        friend void GeneticAlgorithm::astar_danger_refiner(Individ& ind);
        friend std::optional<std::pair<Individ, Individ>> GeneticAlgorithm::crossover(const Individ&, const Individ&);
        friend std::list<Cell> GeneticAlgorithm::get_result(const Individ& ind) const;
    };

    public:
        GeneticAlgorithm(const Map& grid, const EnvironmentOptions& env, std::unique_ptr<DangerObjective>&&);
        ~GeneticAlgorithm() override;
        SearchResult startSearch(ILogger *Logger, const Map &Map, const EnvironmentOptions &options) override;
};

#endif