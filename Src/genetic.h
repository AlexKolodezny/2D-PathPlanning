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

struct GeneticNode : Cell {
};

class GeneticAlgorithm : public Search
{
    const Map& grid;
    std::mt19937 random_generator;
    const EnvironmentOptions& env;

public:
    class Individ;

private:
    void initialize_length_and_danger(Individ& ind) const;
    void length_refiner(Individ& ind);
    void danger_refiner(Individ& ind);
    void mutation(Individ& ind);
    void remove_cycles(Individ& ind) const;
    bool invalid_refiner(Individ& ind);
    std::optional<std::pair<Individ, Individ>> crossover(const Individ& parent1, const Individ& parent2);
    std::list<Cell> get_result(const Individ& ind) const;

    std::list<GeneticNode> line_path(std::pair<int, int> start, std::pair<int, int> end);
    std::list<GeneticNode> random_path(std::pair<int, int> start, std::pair<int, int> end);

    Individ* choose_parent(std::vector<Individ>& generation);
    void nsga_ii(std::vector<Individ>&);
    bool at_random(double p);

public:
    class Individ {
        std::list<GeneticNode> path;
        double length;
        double danger;
        int cross_obstacles;

        Individ(const GeneticAlgorithm& search, const Individ& parent1, std::list<GeneticNode>::const_iterator it1, const Individ& patent2, std::list<GeneticNode>::const_iterator it2);

        Individ& operator=(const Individ&) = default;
        Individ(const Individ&) = default;
    public:
        Individ(GeneticAlgorithm& search, size_t k);

        Individ(Individ&&) = default;
        Individ& operator=(Individ&&) = default;

        Individ copy() const;

        double get_path_length() const;
        double get_path_danger() const;
        bool is_invalid() const;
        int get_crossed_obstacles() const;

        friend void GeneticAlgorithm::initialize_length_and_danger(Individ& ind) const;
        friend void GeneticAlgorithm::length_refiner(Individ& ind);
        friend void GeneticAlgorithm::danger_refiner(Individ& ind);
        friend void GeneticAlgorithm::mutation(Individ& ind);
        friend void GeneticAlgorithm::remove_cycles(Individ& ind) const;
        friend bool GeneticAlgorithm::invalid_refiner(Individ& ind);
        friend std::optional<std::pair<Individ, Individ>> GeneticAlgorithm::crossover(const Individ&, const Individ&);
        friend std::list<Cell> GeneticAlgorithm::get_result(const Individ& ind) const;
    };

    public:
        GeneticAlgorithm(const Map& grid, const EnvironmentOptions& env);
        ~GeneticAlgorithm() override;
        SearchResult startSearch(ILogger *Logger, const Map &Map, const EnvironmentOptions &options) override;
};

#endif