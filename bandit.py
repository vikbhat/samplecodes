import os
import sys
import numpy as np
import random
import math
import matplotlib.pyplot as plt


class Bandit:

    # run multiple bandits and generate averages for given selection functions
    def run_bandits(self,no_arms,runs,steps,sel_funcs):
        results = []
        for func in sel_funcs:
            avg_rewards = np.array([0] * steps)
            avg_opts = np.array([0] * steps)
            for i in xrange(runs):
                arm_rewards = np.array([random.gauss(0, 1) for x in range(no_arms)])
                crew, copts = self.bandit(arm_rewards,func,steps)
                avg_rewards = avg_rewards + crew
                avg_opts = avg_opts + copts
            results.append((avg_rewards / runs, 100 * avg_opts / float(runs)))
        return results



    # generates the results of greedy,softmax and UCB
    def generate_results(self, no_arms, runs=2000, steps=1000):
        eps = [0,0.1,0.01]
        g_labels = []
        g_sel_funcs =[]
        for e in eps :
            g_labels.append('$\epsilon = %f$'%e)
        g_sel_funcs.append(lambda x, y, z: self.epsilon_greedy_selection(x, y, z, eps[0]))
        g_sel_funcs.append(lambda x, y, z: self.epsilon_greedy_selection(x, y, z, eps[1]))
        g_sel_funcs.append(lambda x, y, z: self.epsilon_greedy_selection(x, y, z, eps[2]))
        greedy_results = self.run_bandits(no_arms,runs,steps,g_sel_funcs)
        self.plot_graph('Steps', 'Average Reward',[0,2], [x for x, y in greedy_results], g_labels,outfile='greedy_rewards.png')
        self.plot_graph('Steps', '% Optimal Reward',[0,100], [y for x, y in greedy_results], g_labels,outfile='greedy_opt.png')
        temps = [1,0.3,0.001]
        soft_sel_funcs = []
        softmax_labels = []
        for t in temps :
            softmax_labels.append('$T = %f$'%t)
        soft_sel_funcs.append(lambda x, y, z: self.softmax_selection(x, y, z, temp=temps[0]))
        soft_sel_funcs.append(lambda x, y, z: self.softmax_selection(x, y, z, temp=temps[1]))
        soft_sel_funcs.append(lambda x, y, z: self.softmax_selection(x, y, z, temp=temps[2]))
        softmax_results = self.run_bandits(no_arms,runs,steps,soft_sel_funcs)
        self.plot_graph('Steps', 'Average Reward',[0,2],[x for x, y in softmax_results], softmax_labels,outfile='softmax_rewards.png')
        self.plot_graph('Steps', '% Optimal Reward',[0,100],[y for x, y in softmax_results], softmax_labels,outfile='softmax_opt.png')
        T = 0.3
        eps = 0.1
        all_sel_funcs  = []
        all_sel_funcs.append(lambda x,y,z : self.epsilon_greedy_selection(x,y,z,eps))
        all_sel_funcs.append(lambda x,y,z : self.softmax_selection(x,y,z,T))
        all_sel_funcs.append(lambda x,y,z : self.UCB1(x,y,z))
        all_results = self.run_bandits(no_arms,runs,steps,all_sel_funcs)
        all_labels = ['$\epsilon$-greedy , $\epsilon = 0.1$ ','softmax , $T = 0.3$','UCB1 , $c=2$']
        self.plot_graph('Steps', 'Average Reward',[0,2],[x for x, y in all_results], all_labels,'UCB_rewards.png')
        self.plot_graph('Steps', '% Optimal Reward',[0,100],[y for x, y in all_results],all_labels,'UCB_opts.png')


    # method for epsilon-greedy selection
    @staticmethod
    def epsilon_greedy_selection(avg_rewards, counts, steps, eps):
        total_arms = len(avg_rewards)
        pr = random.random()
        if pr >= eps:
            idx = np.argmax(avg_rewards)
        else:
            idx = random.randint(0, total_arms - 1)
        return idx

    # method computes softmax of given array
    @staticmethod
    def softmax(z):
        m = np.amax(z)
        z = np.exp(z - m)
        return z / np.sum(z)

    # method that selects arms using gibbs distribution
    @staticmethod
    def softmax_selection(avg_rewards, counts, steps, temp):
        probs = Bandit().softmax(avg_rewards / temp)
        csum = np.cumsum(probs)
        pr = random.random()
        idx = 0
        for i in xrange(len(csum)):
            if csum[i] >= pr:
                idx = i
                break
        return idx


    # method that selects arm using UCB1
    @staticmethod
    def UCB1(avg_rewards, counts, steps):
        adjusted_rewards = np.array([r + 2*math.sqrt(2 * math.log(math.e, steps) / max(c,1e-40)) for r, c in zip(avg_rewards, counts)])
        return np.argmax(adjusted_rewards)


    # method to run generalized n-arm bandit using input selection method
    def bandit(self, arm_rewards, decision_func, steps=1000):
        rewards = []
        total_arms = len(arm_rewards)
        avg_rewards = np.array([0.0] * (total_arms))
        counts = np.array([0] * (total_arms))
        optimals = []
        optimal_arm = np.argmax(arm_rewards)
        for i in xrange(steps):
            curra = decision_func(avg_rewards, counts, steps)
            curr_rew = arm_rewards[curra] + random.gauss(0,1)
            counts[curra] += 1
            avg_rewards[curra] = avg_rewards[curra] + (curr_rew - avg_rewards[curra]) / float(counts[curra])
            rewards.append(curr_rew)
            optimals.append(int(curra == optimal_arm))
        return (np.array(rewards), np.array(optimals))


    # plot the optimal percent and average reward graphs
    def plot_graph(self, xlabel, ylabel, ylimit, results, labels, outfile=''):
        xs = [i for i in range(len(results[0]))]
        for ys,l in zip(results,labels):
            plt.plot(xs, ys,label=l)
        plt.legend(loc='lower right')
        #plt.legend(loc='best')
        plt.xlabel(xlabel)
        plt.ylabel(ylabel)
        axes = plt.gca()
        axes.set_xlim([-1, len(results[0])])
        axes.set_ylim(ylimit)
        plt.show()


if __name__ == '__main__':
    bdt = Bandit()
    bdt.generate_results(no_arms=10,runs=200,steps=1000)
