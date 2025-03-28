# Particl: a particle simulator

The main gaol is to be able to support many backends with a single api.

The idea is to implement a base class with methods hiding platform specific implementation.

At first implement a simple grid. Then implement a more advanced GPU sorting infinite algorithm.

Once the base compute infrastructure is implemented, add more features like constraints and different forces and force fields.