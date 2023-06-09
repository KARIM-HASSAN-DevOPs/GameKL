import argparse
import pickle
import time
from collections import Counter
from datetime import timedelta
from itertools import combinations, product
from random import shuffle
from statistics import mean
from subprocess import Popen, PIPE
from sys import stdout


SAME_BOARD_DRAW = 20


results = []
draws = 0


class Results:
    def __init__(self, initial_state, players, winner, loser, turns, time):
        self.initial_state = initial_state
        self.players = players
        self.winner = winner
        self.loser = loser
        self.turns = turns
        self.time = time

    def __repr__(self):
        if self.winner == self.players[0]:
            winner_order = 'P1'
            loser_order = 'P2'
        else:
            winner_order = 'P2'
            loser_order = 'P1'
        return f'{winner_order} {self.winner} beat {loser_order} {self.loser} in {self.turns} turns ({self.time:.2f}s)'


class Engine:
    def __init__(self, cmd):
        self.cmd = cmd
        self.times = []

    def run(self, args, record_time=False):
        start = time.perf_counter()

        p = Popen(self.cmd.split() + args.split(), stdout=PIPE, stderr=PIPE)
        ret = p.wait()
        stderr = p.stderr.read().decode('utf-8').strip()

        if ret != 0:
            print(f'\nError code {ret}')
            input(stderr)

        if record_time:
            self.times.append(time.perf_counter() - start)

        return (
            p.stdout.read().decode('utf-8').strip(),
            stderr,
        )

    def move(self, state):
        _, stderr = self.run(state, record_time=True)

        lines = stderr.split('\n')
        state = lines[-1]
        winner = False
        if state.startswith('winner:'):
            winner = True
            state = lines[-2]

        return state, winner

    def __repr__(self):
        return self.cmd


def play_game(engine1, engine2, initial_state):
    start = time.perf_counter()

    engines = [engine1, engine2]
    state = initial_state
    board = state[:86]

    turns = 0
    same_board_turns = 0
    while True:
        engine = engines.pop(0)
        engines.append(engine)

        turns += 1
        stdout.write((f'\r{engine1} versus {engine2} turn {turns}'))
        state, winner = engine.move(state)

        if board == state[:86]:
            same_board_turns += 1
            if same_board_turns > SAME_BOARD_DRAW:
                print(f'\n{engine1} drew {engine2}. Playing again...')
                global draws
                draws += 1
                return play_game(engine1, engine2, initial_state)
        else:
            board = state[:86]
            same_board_turns = 0

        if winner:
            results.append(Results(
                initial_state=initial_state,
                players=[engine1, engine2],
                winner=engine,
                loser=engines[0],
                turns=turns,
                time=time.perf_counter() - start,
            ))
            print(f'\r{results[-1]}')
            return engine


def print_results():
    engine_wins = Counter()
    engine_losses = Counter()
    engine_p1_wins = Counter()
    engine_p2_wins = Counter()
    player_wins = Counter()
    engine_engine_wins = {}
    engine_engine_p1_wins = {}
    engine_engine_p2_wins = {}
    state_p1_wins = Counter()
    state_p2_wins = Counter()
    state_engine_p1_wins = {}
    state_engine_p2_wins = {}
    state_engine_p1_losses = {}
    state_engine_p2_losses = {}
    for result in results:
        engine_wins[result.winner] += 1
        engine_wins[result.loser] += 0
        engine_losses[result.loser] += 1

        if result.winner not in engine_engine_wins:
            engine_engine_wins[result.winner] = Counter()
            engine_engine_p1_wins[result.winner] = Counter()
            engine_engine_p2_wins[result.winner] = Counter()
        engine_engine_wins[result.winner][result.loser] += 1

        if result.loser not in engine_engine_wins:
            engine_engine_wins[result.loser] = Counter()
            engine_engine_p1_wins[result.loser] = Counter()
            engine_engine_p2_wins[result.loser] = Counter()

        if result.initial_state not in state_engine_p1_wins:
            state_engine_p1_wins[result.initial_state] = Counter()
            state_engine_p2_wins[result.initial_state] = Counter()
            state_engine_p1_losses[result.initial_state] = Counter()
            state_engine_p2_losses[result.initial_state] = Counter()

        if result.winner == result.players[0]:
            engine_p1_wins[result.winner] += 1
            engine_engine_p1_wins[result.winner][result.loser] += 1
            state_p1_wins[result.initial_state] += 1

            state_engine_p1_wins[result.initial_state][result.winner] += 1
            state_engine_p1_wins[result.initial_state][result.loser] += 0
            state_engine_p2_losses[result.initial_state][result.loser] += 1
            state_engine_p2_losses[result.initial_state][result.winner] += 0

        else:
            engine_p2_wins[result.winner] += 1
            engine_engine_p2_wins[result.winner][result.loser] += 1
            state_p1_wins[result.initial_state] += 0
            state_p2_wins[result.initial_state] += 1

            state_engine_p2_wins[result.initial_state][result.winner] += 1
            state_engine_p2_wins[result.initial_state][result.loser] += 0
            state_engine_p1_losses[result.initial_state][result.loser] += 1
            state_engine_p1_losses[result.initial_state][result.winner] += 0

        player_wins[result.players.index(result.winner) + 1] += 1

    i = 0
    matchup_lines = []
    for engine, _ in engine_wins.most_common():
        i += 1
        stdout.write(f'({i:d})\t\t')

        if engine not in engine_engine_wins:
            matchup_lines.append('\n')
            continue

        matchup_line = ''
        for matchup, _ in engine_wins.most_common():
            wins = engine_engine_wins[engine].get(matchup, 0)
            p1_wins = engine_engine_p1_wins[engine].get(matchup, 0)
            p2_wins = engine_engine_p2_wins[engine].get(matchup, 0)
            matchup_line += f' {wins:d}\t'
            matchup_line += f'{p1_wins}:{p2_wins}\t'

        matchup_line += f'({i})\t{engine}'

        matchup_lines.append(matchup_line)
    print()
    print('\n'.join(matchup_lines))

    print('\nP1\tP2\tState')
    for state, _ in state_p1_wins.most_common():
        print(f'{state_p1_wins[state]}\t{state_p2_wins[state]}\t{state}')

        stdout.write(f'\t')
        i = -1
        for engine, _ in engine_wins.most_common():
            i += 1
            stdout.write(f'\t({i}) {state_engine_p1_wins[state][engine]}:{state_engine_p2_wins[state][engine]}')
            stdout.write(f'/{state_engine_p1_losses[state][engine]}:{state_engine_p2_losses[state][engine]}')

        print()

    print('\nWins\tP1:P2\tLosses\tTime\tEngine')
    for engine, wins in engine_wins.most_common():
        print(
            f'{wins}\t{engine_p1_wins[engine]}:{engine_p2_wins[engine]}\t{engine_losses[engine]}\t{mean(engine.times):.1f}s\t{engine}')
    print(f'\t{player_wins[1]}:{player_wins[2]}')

    print(f'\nDraws: {draws}\n')


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('-e', '--engine', action='append', required=True)
    parser.add_argument('-n', '--game-pairs', type=int, default=10)
    parser.add_argument('-r', '--repeat-pairs', type=int, default=1)
    parser.add_argument('-w', '--results-file', type=argparse.FileType('wb'))
    args = parser.parse_args()

    if len(args.engine) == 1:
        print('Warning: no games to play with only one engine')

    engines = []
    for engine_cmd in args.engine:
        engines.append(Engine(engine_cmd))

    initial_states = []

    stdout, _ = engines[0].run(f'-g {args.game_pairs}')
    for initial_state in stdout.split():
        initial_states.append(initial_state)

    matchups = list(combinations(engines, 2))
    matchups = list(product(matchups, initial_states * args.repeat_pairs))
    total_games = 2 * len(matchups)
    played_games = 0

    shuffle(matchups)
    start_time = time.perf_counter()
    for ((engine1, engine2), initial_state) in matchups:
        for p1, p2 in [(engine1, engine2), (engine2, engine1)]:
            play_game(p1, p2, initial_state)
            played_games += 1

            percentage = 100 * played_games / total_games
            mean_turns = mean([result.turns for result in results])
            mean_time = mean([result.time for result in results])
            remaining_games = total_games - played_games
            elapsed = timedelta(seconds=int(time.perf_counter() - start_time))
            etr = timedelta(seconds=int(mean_time * remaining_games))

            print()
            print_results()
            print(f'{played_games}/{total_games}\t{percentage:.0f}%\tavg. game {mean_turns:.0f}t/{mean_time:.1f}s\t{elapsed} elapsed\t\tapprox. {etr} remaining')

            if args.results_file:
                args.results_file.seek(0)
                pickle.dump(results, args.results_file)


if __name__ == '__main__':
    main()
