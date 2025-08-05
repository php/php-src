<?php

function check_equivalence($tests)
{
    foreach ($tests as $test => [$pfa, $closure]) {
        echo "# ", $test, ": ";
        $pfaReflector = new ReflectionFunction($pfa);
        $closureReflector = new ReflectionFunction($closure);

        try {
            if (count($pfaReflector->getParameters()) !== count($closureReflector->getParameters())) {
                throw new Exception(sprintf(
                    "Arity does not match: expected %d, got %d",
                    count($closureReflector->getParameters()),
                    count($pfaReflector->getParameters()),
                ));
            }

            $it = new MultipleIterator();
            $it->attachIterator(new ArrayIterator($pfaReflector->getParameters()));
            $it->attachIterator(new ArrayIterator($closureReflector->getParameters()));
            foreach ($it as $i => [$pfaParam, $closureParam]) {
                [$i] = $i;
                if ($pfaParam->getName() !== $closureParam->getName()) {
                    throw new Exception(sprintf("Name of param %d does not match: %s vs %s",
                        $i,
                        $pfaParam->getName(),
                        $closureParam->getName(),
                    ));
                }
                if ((string)$pfaParam->getType() !== (string)$closureParam->getType()) {
                    throw new Exception(sprintf("Type of param %d does not match: %s vs %s",
                        $i,
                        $pfaParam->getType(),
                        $closureParam->getType(),
                    ));
                }
                if ($pfaParam->isOptional() !== $closureParam->isOptional()) {
                    throw new Exception(sprintf("Optionalness of param %d does not match: %d vs %d",
                        $i,
                        $pfaParam->isOptional(),
                        $closureParam->isOptional(),
                    ));
                }
            }

            $args = [];
            foreach ($pfaReflector->getParameters() as $i => $p) {
                $args[] = match ((string) $p->getType()) {
                    'int' => 100 + $i,
                    'float' => 100.5 + $i,
                    '?float' => 100.5 + $i,
                    'string' => (string) (100 + $i),
                    'Point' => new Point,
                    '' => "mixed($i)",
                };
            }

            if ($pfaReflector->getClosureThis() !== $closureReflector->getClosureThis()) {
                throw new Exception("\$this differs");
            }

            if ($pfa(...$args) !== $closure(...$args)) {
                throw new Exception("PFA is not equivalent to closure");
            }
        } catch (Exception $e) {
            echo $e->getMessage(), "\n";
            echo $pfaReflector;
            echo $closureReflector;
            return;
        }

        echo "Ok\n";
    }
}
