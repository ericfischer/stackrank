#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <map>
#include <set>
#include <string>

struct report {
	std::string a;
	std::string b;
	int diff;
};

int main(int argc, char **argv) {
	if (argc > 1) {
		srand(atoi(argv[1]));
	}

	FILE *asked = fopen("asked", "r");
	if (asked == NULL) {
		perror("fopen");
		exit(EXIT_FAILURE);
	}

	std::map<std::string, size_t> ranks;
	std::vector<std::string> order;
	std::vector<report> reports;
	std::map<std::pair<std::string, std::string>, size_t> reportmap;
	std::set<std::string> rejects;

	std::multimap<int, std::string> inputs;

	char s[2000];
	while (fgets(s, 2000, asked)) {
		inputs.insert(std::pair<int, std::string>(rand(), s));

		char one[2000];
		char two[2000];
		char cmp[2000];

		if (sscanf(s, "%s %s %s", one, two, cmp) != 3) {
			continue;
		}

		if (strcmp(cmp, "RejectA0First") == 0) {
			rejects.insert(one);
		}
		if (strcmp(cmp, "RejectA0Second") == 0) {
			rejects.insert(two);
		}
	}

	for (auto a : inputs) {
		const char *s = a.second.c_str();
		char one[2000];
		char two[2000];
		char cmp[2000];

		if (sscanf(s, "%s %s %s", one, two, cmp) != 3) {
			fprintf(stderr, "??? %s", s);
			continue;
		}

		if (rejects.count(one)) {
			continue;
		}
		if (rejects.count(two)) {
			continue;
		}

		int diff = 0;
		if (strcmp(cmp, "First") == 0) {
			diff = 1;
		} else if (strcmp(cmp, "Second") == 0) {
			diff = -1;
		} else {
			fprintf(stderr, "??? %s\n", cmp);
			continue;
		}

		std::pair<std::string, std::string> k = std::pair<std::string, std::string>(one, two);
		std::pair<std::pair<std::string, std::string>, size_t> p = std::pair<std::pair<std::string, std::string>, size_t>(k, reports.size());
		reportmap.insert(p);

		report r;
		r.a = one;
		r.b = two;
		r.diff = diff;
		reports.push_back(r);

		if (ranks.count(one) == 0) {
			ranks.insert(std::pair<std::string, size_t>(one, order.size()));
			order.push_back(one);
		}

		if (ranks.count(two) == 0) {
			ranks.insert(std::pair<std::string, size_t>(two, order.size()));
			order.push_back(two);
		}
	}

	while (1) {
		size_t errors = 0;

		for (size_t i = 0; i < reports.size(); i++) {
			int diff = ranks.find(reports[i].a)->second - ranks.find(reports[i].b)->second;

			if ((diff < 0) != (reports[i].diff < 0)) {
				errors++;

				auto fa = ranks.find(reports[i].a);
				auto fb = ranks.find(reports[i].b);
				size_t a = fa->second;
				size_t b = fb->second;

				ranks.erase(fa);
				ranks.erase(fb);

				ranks.insert(std::pair<std::string, size_t>(reports[i].a, b));
				ranks.insert(std::pair<std::string, size_t>(reports[i].b, a));
				order[a] = reports[i].b;
				order[b] = reports[i].a;
			}
		}

		fprintf(stderr, "%zu\n", errors);

		if (errors == 0) {
			break;
		}
	}

	for (size_t i = 0; i < order.size(); i++) {
		printf("%s", order[i].c_str());

		if (i + 1 < order.size()) {
			if (reportmap.count(std::pair<std::string, std::string>(order[i], order[i + 1])) ||
			    reportmap.count(std::pair<std::string, std::string>(order[i + 1], order[i]))) {
				printf("\n");
			} else {
				printf("\n");
			}
		} else {
			printf("\n");
		}
	}
}
