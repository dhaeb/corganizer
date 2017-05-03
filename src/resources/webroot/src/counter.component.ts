import { Component } from '@angular/core';

@Component({
  selector: 'counter',
  styles: [`
    .counter {
      position: relative;
    }
    .counter__input {
      border: 0;
      border-radius: 3px;
      height: 30px;
      max-width: 100px;
      text-align: center;
    }
    .counter__button {
      outline: 0;
      cursor: pointer;
      height: 30px;
      width: 30px;
      border: 0;
      border-radius: 3px;
      background: #0088cc;
      color: #fff;
    }
  `],
  template: `
    <div class="counter">
      <div class="counter__container">
        <button (click)="decrement();" class="counter__button">
          -
        </button>
        <input type="text" class="counter__input" [value]="counterValue">
        <button (click)="increment();" class="counter__button">
          +
        </button>
      </div>
    </div>
  `
})
export class CounterComponent {
  counterValue: number = 0;
  increment() {
    this.counterValue++;
  }
  decrement() {
    this.counterValue--;
  }
}